# -*- coding: utf-8 -*-
# 仅解析本次采样；MAP 使用本次 Release 的地址，未解析的系统栈保留模块偏移。
import argparse, bisect, collections, contextlib, csv, ctypes, html, json, re
from pathlib import Path
ROOT = Path(__file__).resolve().parent
parser = argparse.ArgumentParser(description='解析本次主线程采样；必须使用采样构建对应的 MAP。')
parser.add_argument('--prefix', default='release')
parser.add_argument('--map-file', type=Path)
parser.add_argument('--wall-only', action='store_true')
args = parser.parse_args()
if args.wall_only and args.map_file is None:
    parser.error('仅解析周期栈时须显式指定该构建保存的 --map-file。')
wall = json.loads((ROOT / f'{args.prefix}_main_thread_wall.json').read_text(encoding='utf-8-sig'))
modules = sorted([(int(m['baseAddress'],16), int(m['size']), m['name']) for m in wall['modules']])
module_bases = [m[0] for m in modules]
client_base = next(m[0] for m in modules if m[2].lower() == 'cg2d_re_client.exe')
map_file = args.map_file or ROOT.parents[2] / 'build/windows-msvc-x64-release/cg2d_re_client.map'
map_text = map_file.read_text(encoding='utf-8')
map_timestamp = int(re.search(r'Timestamp is ([0-9a-fA-F]+)', map_text)[1], 16)
undecorate = ctypes.WinDLL('dbghelp').UnDecorateSymbolName
undecorate.argtypes = [ctypes.c_char_p, ctypes.c_void_p, ctypes.c_uint, ctypes.c_uint]
undecorate.restype = ctypes.c_uint
funcs = {}
for line in map_text.splitlines():
    found = re.match(r'\s*0001:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{16})\s+', line)
    if found:
        rva = int(found[2],16) - 0x140000000
        name = found[1]
        if name.startswith('?'):
            buffer = ctypes.create_string_buffer(2048)
            if undecorate(name.encode('ascii'),buffer,len(buffer),0x1000): name = buffer.value.decode('ascii')
        funcs.setdefault(rva,name)
func_rvas = sorted(funcs)
cache = {}
def label(addr):
    if addr in cache: return cache[addr]
    index = bisect.bisect_right(module_bases,addr)-1
    if index < 0 or addr >= modules[index][0]+modules[index][1]: return '未知模块!0x%x' % addr
    base,size,module = modules[index]
    offset = addr-base
    if module.lower() == 'cg2d_re_client.exe':
        ix = bisect.bisect_right(func_rvas,offset)-1
        name = funcs[func_rvas[ix]] if ix >= 0 else 'RVA_0x%x' % offset
        value = name
    else:
        name = wall['symbols'].get('0x%x' % addr,'')
        value = module+'!'+(name if name and int(name.rsplit('+0x',1)[-1],16) <= 1024 else 'RVA_0x%x' % offset)
    cache[addr]=value
    return value
wall_folded=collections.Counter(); wall_groups=collections.Counter(); wall_leaf=collections.Counter()
wall_costs=[]
for sample in wall['samples']:
    names=[label(int(a,16)) for a in sample['addresses']]
    wall_folded[tuple(reversed(names))]+=1
    wall_leaf[names[0]]+=1
    for name in set(names): wall_groups[name]+=1
    wall_costs.append(sample['suspensionMs'])
main_tid=wall['threadId']; pid=wall['processId']
cpu_folded=collections.Counter(); cpu_leaf=collections.Counter(); cpu_groups=collections.Counter(); thread_counts=collections.Counter()
pending=None; raw_samples=0; main_samples=0; start_us=40_000_000; end_us=60_000_000

def flush():
    global pending, raw_samples, main_samples
    if pending is None: return
    tid,count,frames=pending
    thread_counts[tid]+=count; raw_samples+=count
    if tid == main_tid:
        main_samples+=count
        names=[label(a) for a in frames]
        if names:
            cpu_folded[tuple(reversed(names))]+=count
            cpu_leaf[names[0]]+=count
            for name in set(names): cpu_groups[name]+=count
    pending=None

image_timestamp = None
with (ROOT/f'{args.prefix}_trace_events.csv').open(encoding='utf-8-sig',newline='') if not args.wall_only else contextlib.nullcontext([]) as stream:
    for row in csv.reader(stream,skipinitialspace=True):
        if not row: continue
        event=row[0].strip()
        if event=='ImageId' and len(row)>5 and row[2].strip()==f'cg2d_re_client.exe ({pid})':
            image_timestamp = int(row[5], 16)
        if event=='Stack' and pending is not None and len(row)>4:
            if int(row[2]) == pending[0]: pending[2].append(int(row[4],16))
            continue
        flush()
        if event=='SampledProfile' and len(row)>8 and row[2].strip()==f'cg2d_re_client.exe ({pid})':
            timestamp=int(row[1])
            if start_us <= timestamp <= end_us:
                pending=[int(row[3]),int(row[8]),[]]
    flush()
if not args.wall_only and image_timestamp != map_timestamp:
    raise SystemExit('MAP 时间戳与追踪中客户端不符；禁止用重新构建后的 MAP 覆盖旧采样结果。')

for kind,folded in ([('wall',wall_folded)] if args.wall_only else [('cpu',cpu_folded),('wall',wall_folded)]):
    (ROOT/f'{args.prefix}_main_{kind}.folded').write_text(''.join(';'.join(k)+' '+str(v)+'\n' for k,v in sorted(folded.items())),encoding='utf-8',newline='\n')
    tree={'name':'主线程','count':0,'children':{}}
    for stack,count in folded.items():
        node=tree; node['count']+=count
        for name in stack:
            node=node['children'].setdefault(name,{'name':name,'count':0,'children':{}})
            node['count']+=count
    def depth(n): return 1+max((depth(c) for c in n['children'].values()),default=0)
    width=1500; row_height=21; height=depth(tree)*row_height+110
    title_prefix={'release_timer1':'Release 1ms 对照','release_software':'Release 软件后端','release_auto_repeat':'Release 自动后端回测','release_fixed':'Release D3D9 候选（已撤回）'}.get(args.prefix,'Release')
    total=tree['count']; title=title_prefix+' 主线程 '+('CPU 火焰图（运行样本）' if kind=='cpu' else '等待与运行火焰图（周期栈样本）')
    detail = '本图为 12 秒周期采样。' if args.wall_only else 'CPU 图范围为采集第 40–60 秒；等待图为后续 12 秒周期采样。'
    svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">', '<rect width="100%" height="100%" fill="#fafafa"/>', '<style>text{font-family:Consolas,"Microsoft YaHei",sans-serif}g:hover rect{stroke:#222;stroke-width:1.5}</style>',f'<text x="16" y="29" font-size="21">{title}</text>',f'<text x="16" y="53" font-size="13">样本数：{total}；宽度表示样本占比。悬停查看完整调用与数量。{detail}</text>']
    def render(n,x,y,w):
        if w<0.7: return
        color='#f1b55b' if kind=='cpu' else '#85bddc'
        tooltip=html.escape(f'{n["name"]}\n{n["count"]} 样本，{n["count"]/total*100:.2f}%')
        svg.append(f'<g><title>{tooltip}</title><rect x="{x:.3f}" y="{y}" width="{w:.3f}" height="{row_height-1}" fill="{color}" stroke="white" stroke-width="0.5"/>')
        if w>35:
            text=n['name'][:max(2,int(w/7)-1)]
            svg.append(f'<text x="{x+3:.3f}" y="{y+14}" font-size="11">{html.escape(text)}</text>')
        svg.append('</g>')
        cursor=x
        for child in sorted(n['children'].values(),key=lambda c:(-c['count'],c['name'])):
            childw=w*child['count']/n['count']; render(child,cursor,y-row_height,childw); cursor+=childw
    render(tree,10,height-30,width-20)
    svg.append('</svg>'); (ROOT/f'{args.prefix}_main_{kind}.svg').write_text('\n'.join(svg),encoding='utf-8',newline='\n')
summary={'pid':pid,'main_thread':main_tid,'map_timestamp':f'{map_timestamp:x}','wall_duration_ms':wall['durationMs'],'wall_samples':len(wall['samples']),'wall_cpu_seconds':wall['cpuSecondsAfter']-wall['cpuSecondsBefore'],'sampler_suspension_ms_total':sum(wall_costs),'sampler_suspension_ms_max':max(wall_costs),'wall_leaf_top':wall_leaf.most_common(12),'wall_inclusive_top':wall_groups.most_common(24)}
if not args.wall_only:
    summary.update({'cpu_stable_range_seconds':[40,60],'cpu_samples_all_threads':raw_samples,'cpu_samples_main_thread':main_samples,'cpu_thread_counts':thread_counts,'cpu_leaf_top':cpu_leaf.most_common(16),'cpu_inclusive_top':cpu_groups.most_common(24)})
(ROOT/f'{args.prefix}_summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2),encoding='utf-8',newline='\n')
print(json.dumps(summary,ensure_ascii=False,indent=2))
