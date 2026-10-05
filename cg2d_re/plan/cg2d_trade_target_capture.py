# -*- coding: utf-8 -*-
"""TTP01：旧交易名单生产取证；须按配套SPEC取得许可，定义/构造不安装断点。
通过runpy装载此文件；不启动/附加/继续进程，不保存数据库或写运行证据。
"""

import runpy
from pathlib import Path


_CaptureBase = runpy.run_path(
    str(Path(__file__).with_name("cg2d_pet_catalog_capture.py"))
)["PetCatalogCapture"]


class TradeTargetCapture(_CaptureBase):
    """原三个字段定点写监视；单条17B快照不证明数组容量。"""

    TARGETS = (
        (0xCB1D1C, 1, "首条名称首字节"),
        (0xD30134, 4, "名单总数"),
        (0xCF46D0, 4, "名单页数"),
    )
    SNAPSHOTS = ((0xCB1D1C, 17, "首条17B"),)
