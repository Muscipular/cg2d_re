#ifndef CG_ENGINE_PACKET_PARSER_H
#define CG_ENGINE_PACKET_PARSER_H

#include "PacketTypes.hpp"
#include "utils.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
    struct PacketFieldSpec
    {
        std::string_view name;
        PacketFieldType type = PacketFieldType::Unknown;
    };

    struct PacketSpec
    {
        std::string_view code;
        PacketDirection direction = PacketDirection::ClientToServer;
        const PacketFieldSpec* fields = nullptr;
        std::size_t fieldCount = 0;
    };

    namespace detail
    {
        inline constexpr std::array<PacketFieldSpec, 3> ClientLoginFields = {{
            {"clientHash", PacketFieldType::EscapedString},
            {"password", PacketFieldType::EscapedString},
            {"account", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ClientLoginResultFields = {{
            {"result", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> CharacterListFields = {{
            {"result", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> CreateNewCharResultFields = {{
            {"result", PacketFieldType::EscapedString},
            {"registnumber", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> CharacterDeleteResultFields = {{
            {"result", PacketFieldType::EscapedString},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProcessStatusFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 13> CreateNewCharFields = {{
            {"dataPlace", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
            {"image", PacketFieldType::Int62},
            {"face", PacketFieldType::Int62},
            {"vital", PacketFieldType::Int62},
            {"strength", PacketFieldType::Int62},
            {"tough", PacketFieldType::Int62},
            {"quick", PacketFieldType::Int62},
            {"magic", PacketFieldType::Int62},
            {"earth", PacketFieldType::Int62},
            {"water", PacketFieldType::Int62},
            {"fire", PacketFieldType::Int62},
            {"wind", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileVersionFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterProfileFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterProfile2Fields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> JobNameFields = {{
            {"jobassort", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterSkillDataFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ItemDataFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetItemDataFields = {{
            {"petSlot", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> EquipSuitFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ItemInfoListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ItemRecipeFields = {{
            {"haveskillindex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> AlbumFlagsFields = {{
            {"albumversion", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 17> AlbumInfoFields = {{
            {"albumId", PacketFieldType::Int62},
            {"imageNumber", PacketFieldType::Int62},
            {"catchFlag", PacketFieldType::Int62},
            {"rare", PacketFieldType::Int62},
            {"type", PacketFieldType::Int62},
            {"vital", PacketFieldType::Int62},
            {"strength", PacketFieldType::Int62},
            {"toughness", PacketFieldType::Int62},
            {"quick", PacketFieldType::Int62},
            {"magic", PacketFieldType::Int62},
            {"earth", PacketFieldType::Int62},
            {"water", PacketFieldType::Int62},
            {"fire", PacketFieldType::Int62},
            {"wind", PacketFieldType::Int62},
            {"slot", PacketFieldType::Int62},
            {"comment", PacketFieldType::EscapedString},
            {"name", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> AlbumNameFields = {{
            {"albumId", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 8> PetMailAnimationFields = {{
            {"objIndex", PacketFieldType::Int62},
            {"graphicsno", PacketFieldType::Int62},
            {"x", PacketFieldType::Int62},
            {"y", PacketFieldType::Int62},
            {"dir", PacketFieldType::Int62},
            {"flg", PacketFieldType::Int62},
            {"no", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> TitleListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> AboutListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 13> ProfileDataFields = {{
            {"name", PacketFieldType::EscapedString},
            {"lv", PacketFieldType::Int62},
            {"job", PacketFieldType::EscapedString},
            {"title", PacketFieldType::EscapedString},
            {"guildName", PacketFieldType::EscapedString},
            {"graphic", PacketFieldType::Int62},
            {"sid", PacketFieldType::Int62},
            {"smsg", PacketFieldType::EscapedString},
            {"bid", PacketFieldType::Int62},
            {"bmsg", PacketFieldType::EscapedString},
            {"aid", PacketFieldType::Int62},
            {"amsg", PacketFieldType::EscapedString},
            {"pmsg", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 7> SetProfileDataFields = {{
            {"sellFlag", PacketFieldType::Int62},
            {"sellMsg", PacketFieldType::EscapedString},
            {"buyFlag", PacketFieldType::Int62},
            {"buyMsg", PacketFieldType::EscapedString},
            {"aboutFlg", PacketFieldType::Int62},
            {"aboutMsg", PacketFieldType::EscapedString},
            {"ProfileMsg", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 4> MagicUseRequestFields = {{
            {"x", PacketFieldType::Int62},
            {"y", PacketFieldType::Int62},
            {"array", PacketFieldType::Int62},
            {"toindex", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 4> PetSkillUseRequestFields = {{
            {"havepetindex", PacketFieldType::Int62},
            {"havepetskill", PacketFieldType::Int62},
            {"toindex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetLevelUpPointFields = {{
            {"petSlot", PacketFieldType::Int62},
            {"sqeno", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> PetSkillSwapFields = {{
            {"petSlot", PacketFieldType::Int62},
            {"from", PacketFieldType::Int62},
            {"to", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> UserPetNameInputFields = {{
            {"petSlot", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> MailMessageSendFields = {{
            {"index", PacketFieldType::Int62},
            {"message", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 6> ChatMessageSendFields = {{
            {"x", PacketFieldType::Int62},
            {"y", PacketFieldType::Int62},
            {"msg", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
            {"area", PacketFieldType::Int62},
            {"fontSize", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 6> WindowOpenSendFields = {{
            {"x", PacketFieldType::Int62},
            {"y", PacketFieldType::Int62},
            {"seqno", PacketFieldType::Int62},
            {"objindex", PacketFieldType::Int62},
            {"select", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ShutdownRequestFields = {{
            {"passwd", PacketFieldType::EscapedString},
            {"min", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 5> PetMailSendFields = {{
            {"index", PacketFieldType::Int62},
            {"petindex", PacketFieldType::Int62},
            {"itemindex", PacketFieldType::Int62},
            {"message", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 5> PetMailRegisterFields = {{
            {"toregist", PacketFieldType::Int62},
            {"petSlot", PacketFieldType::Int62},
            {"itemSlot", PacketFieldType::Int62},
            {"text", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileMailDeleteFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> ProfileMailReceiptFields = {{
            {"profileIndex", PacketFieldType::Int62},
            {"dtext", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileMailArriveFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> PartyParamFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PartyMemberStatusFields = {{
            {"partySlot", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> TechListFields = {{
            {"skillIndex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetTechDataFields = {{
            {"petIndex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> RidePetDataFields = {{
            {"petIndex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> TechUseResultFields = {{
            {"result", PacketFieldType::Int62},
            {"type", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileStatusFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> FrontCharListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterPetListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 5> ProfileWindowFields = {{
            {"windowType", PacketFieldType::Int62},
            {"buttonType", PacketFieldType::Int62},
            {"seqNo", PacketFieldType::Int62},
            {"objectIndex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> SystemMessageFields = {{
            {"message", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> EffectFields = {{
            {"EffType", PacketFieldType::Int62},
            {"TM_EffSize", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> BattleCommandFields = {{
            {"command", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> BattleRewardFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> DuelPointFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> ProfileMailMessageFields = {{
            {"aindex", PacketFieldType::Int62},
            {"text", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 4> ChatMessageFields = {{
            {"objectIndex", PacketFieldType::Int62},
            {"message", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
            {"fontSize", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 5> WindowPacketFields = {{
            {"windowtype", PacketFieldType::Int62},
            {"buttontype", PacketFieldType::Int62},
            {"seqno", PacketFieldType::Int62},
            {"objindex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 6> StallBrowseFields = {{
            {"result", PacketFieldType::Int62},
            {"objIndex", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
            {"description", PacketFieldType::EscapedString},
            {"item", PacketFieldType::EscapedString},
            {"pet", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> StallStartFields = {{
            {"stallName", PacketFieldType::EscapedString},
            {"stallDescription", PacketFieldType::EscapedString},
            {"stallList", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> AddressBookFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> AddressBookInfoFields = {{
            {"registry", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> AddressBookGuildInfoFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> GuildInfoFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> GuildTitleFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> GuildMemberFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> GuildMemberInfoFields = {{
            {"index", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> GuildMailFields = {{
            {"fromregistseqnumber", PacketFieldType::Int62},
            {"text", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> TradePlayerListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> TradeStartFields = {{
            {"name", PacketFieldType::EscapedString},
            {"level", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> TradeItemInfoFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> TradePetInfoFields = {{
            {"petPlace", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> TradePetSkillInfoFields = {{
            {"petIndex", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> TradeTakeOutFields = {{
            {"item", PacketFieldType::EscapedString},
            {"pet", PacketFieldType::EscapedString},
            {"gold", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> DropGoldFields = {{
            {"x", PacketFieldType::Int62},
            {"y", PacketFieldType::Int62},
            {"amount", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> GuildInfoChangeFields = {{
            {"c", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> GuildTitleSetFields = {{
            {"index", PacketFieldType::Int62},
            {"bitflag", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> GuildMailSendFields = {{
            {"toregistNumber", PacketFieldType::Int62},
            {"text", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ProfileMailFriendDeleteFields = {{
            {"profileIndex", PacketFieldType::Int62},
            {"name", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ProfileMiniMailSendFields = {{
            {"data", PacketFieldType::EscapedString},
            {"color", PacketFieldType::Int62},
        }};

        inline constexpr PacketSpec PetMailAnimationSpec {
            "lDuJ",
            PacketDirection::ServerToClient,
            PetMailAnimationFields.data(),
            PetMailAnimationFields.size(),
        };

        inline constexpr PacketSpec ProfileWindowSpec {
            "DyAG",
            PacketDirection::ServerToClient,
            ProfileWindowFields.data(),
            ProfileWindowFields.size(),
        };

        inline constexpr PacketSpec BattleRewardSpec {
            "Gn",
            PacketDirection::ServerToClient,
            BattleRewardFields.data(),
            BattleRewardFields.size(),
        };

        inline constexpr PacketSpec ChatMessageSpec {
            "zdr",
            PacketDirection::ServerToClient,
            ChatMessageFields.data(),
            ChatMessageFields.size(),
        };

        inline constexpr PacketSpec AddressBookSpec {
            "gnxd",
            PacketDirection::ServerToClient,
            AddressBookFields.data(),
            AddressBookFields.size(),
        };

        inline constexpr PacketSpec AddressBookInfoSpec {
            "vI",
            PacketDirection::ServerToClient,
            AddressBookInfoFields.data(),
            AddressBookInfoFields.size(),
        };

        inline constexpr PacketSpec AddressBookGuildInfoSpec {
            "PhdQ",
            PacketDirection::ServerToClient,
            AddressBookGuildInfoFields.data(),
            AddressBookGuildInfoFields.size(),
        };

        inline constexpr PacketSpec GuildInfoSpec {
            "aDn",
            PacketDirection::ServerToClient,
            GuildInfoFields.data(),
            GuildInfoFields.size(),
        };

        inline constexpr PacketSpec GuildTitleSpec {
            "znv",
            PacketDirection::ServerToClient,
            GuildTitleFields.data(),
            GuildTitleFields.size(),
        };

        inline constexpr PacketSpec GuildMemberSpec {
            "yuD",
            PacketDirection::ServerToClient,
            GuildMemberFields.data(),
            GuildMemberFields.size(),
        };

        inline constexpr PacketSpec GuildMemberInfoSpec {
            "Kka",
            PacketDirection::ServerToClient,
            GuildMemberInfoFields.data(),
            GuildMemberInfoFields.size(),
        };

        inline constexpr PacketSpec GuildMailSpec {
            "Rll",
            PacketDirection::ServerToClient,
            GuildMailFields.data(),
            GuildMailFields.size(),
        };

        inline constexpr PacketSpec TradePlayerListSpec {
            "Cxo",
            PacketDirection::ServerToClient,
            TradePlayerListFields.data(),
            TradePlayerListFields.size(),
        };

        inline constexpr PacketSpec TradeStartSpec {
            "ms",
            PacketDirection::ServerToClient,
            TradeStartFields.data(),
            TradeStartFields.size(),
        };

        inline constexpr PacketSpec TradeItemInfoSpec {
            "nUbJ",
            PacketDirection::ServerToClient,
            TradeItemInfoFields.data(),
            TradeItemInfoFields.size(),
        };

        inline constexpr PacketSpec TradePetInfoSpec {
            "RwK",
            PacketDirection::ServerToClient,
            TradePetInfoFields.data(),
            TradePetInfoFields.size(),
        };

        inline constexpr PacketSpec TradePetSkillInfoSpec {
            "Yq",
            PacketDirection::ServerToClient,
            TradePetSkillInfoFields.data(),
            TradePetSkillInfoFields.size(),
        };

        inline constexpr PacketSpec ProfileMiniMailSendSpec {
            "GH",
            PacketDirection::ClientToServer,
            ProfileMiniMailSendFields.data(),
            ProfileMiniMailSendFields.size(),
        };

        inline constexpr std::array<PacketFieldSpec, 1> CheckCommandFields = {{
            {"time", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> MapNameFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ObjectListFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterIconFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ObjectDeleteFields = {{
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 7> MapDataFields = {{
            {"mapid", PacketFieldType::Int62},
            {"floor", PacketFieldType::Int62},
            {"x1", PacketFieldType::Int62},
            {"y1", PacketFieldType::Int62},
            {"x2", PacketFieldType::Int62},
            {"y2", PacketFieldType::Int62},
            {"data", PacketFieldType::EscapedString},
        }};

        inline constexpr std::array<PacketFieldSpec, 6> MapDataRequestFields = {{
            {"mapid", PacketFieldType::Int62},
            {"floor", PacketFieldType::Int62},
            {"x1", PacketFieldType::Int62},
            {"y1", PacketFieldType::Int62},
            {"x2", PacketFieldType::Int62},
            {"y2", PacketFieldType::Int62},
        }};

        inline constexpr std::array<PacketSpec, 91> PacketRegistry = {{
            {
                "JFVf",
                PacketDirection::ClientToServer,
                ClientLoginFields.data(),
                ClientLoginFields.size(),
            },
            {
                "whV",
                PacketDirection::ServerToClient,
                ClientLoginResultFields.data(),
                ClientLoginResultFields.size(),
            },
            {
                "Daj",
                PacketDirection::ServerToClient,
                CharacterListFields.data(),
                CharacterListFields.size(),
            },
            {
                "DZ",
                PacketDirection::ServerToClient,
                CreateNewCharResultFields.data(),
                CreateNewCharResultFields.size(),
            },
            {
                "oy",
                PacketDirection::ServerToClient,
                CharacterDeleteResultFields.data(),
                CharacterDeleteResultFields.size(),
            },
            {
                "cJzI",
                PacketDirection::ServerToClient,
                ProcessStatusFields.data(),
                ProcessStatusFields.size(),
            },
            {
                "aSEl",
                PacketDirection::ClientToServer,
                CreateNewCharFields.data(),
                CreateNewCharFields.size(),
            },
            {
                "HNP",
                PacketDirection::ServerToClient,
                ProfileVersionFields.data(),
                ProfileVersionFields.size(),
            },
            {
                "aPc",
                PacketDirection::ServerToClient,
                CharacterProfileFields.data(),
                CharacterProfileFields.size(),
            },
            {
                "UU",
                PacketDirection::ServerToClient,
                CharacterProfile2Fields.data(),
                CharacterProfile2Fields.size(),
            },
            {
                "jv",
                PacketDirection::ServerToClient,
                JobNameFields.data(),
                JobNameFields.size(),
            },
            {
                "cNE",
                PacketDirection::ServerToClient,
                CharacterSkillDataFields.data(),
                CharacterSkillDataFields.size(),
            },
            {
                "rU",
                PacketDirection::ServerToClient,
                ItemDataFields.data(),
                ItemDataFields.size(),
            },
            {
                "iKe",
                PacketDirection::ServerToClient,
                PetItemDataFields.data(),
                PetItemDataFields.size(),
            },
            {
                "TgX",
                PacketDirection::ServerToClient,
                EquipSuitFields.data(),
                EquipSuitFields.size(),
            },
            {
                "ELoy",
                PacketDirection::ServerToClient,
                ItemInfoListFields.data(),
                ItemInfoListFields.size(),
            },
            {
                "xmgQ",
                PacketDirection::ServerToClient,
                ItemRecipeFields.data(),
                ItemRecipeFields.size(),
            },
            {
                "Ilz",
                PacketDirection::ServerToClient,
                AlbumFlagsFields.data(),
                AlbumFlagsFields.size(),
            },
            {
                "dM",
                PacketDirection::ServerToClient,
                AlbumInfoFields.data(),
                AlbumInfoFields.size(),
            },
            {
                "Dn",
                PacketDirection::ServerToClient,
                AlbumNameFields.data(),
                AlbumNameFields.size(),
            },
            PetMailAnimationSpec,
            {
                "Sc",
                PacketDirection::ServerToClient,
                TitleListFields.data(),
                TitleListFields.size(),
            },
            {
                "eZT",
                PacketDirection::ServerToClient,
                ProfileListFields.data(),
                ProfileListFields.size(),
            },
            {
                "IUn",
                PacketDirection::ServerToClient,
                AboutListFields.data(),
                AboutListFields.size(),
            },
            {
                "DpSY",
                PacketDirection::ServerToClient,
                ProfileDataFields.data(),
                ProfileDataFields.size(),
            },
            {
                "Ot",
                PacketDirection::ClientToServer,
                SetProfileDataFields.data(),
                SetProfileDataFields.size(),
            },
            {
                "RXr",
                PacketDirection::ClientToServer,
                MagicUseRequestFields.data(),
                MagicUseRequestFields.size(),
            },
            {
                "Flvp",
                PacketDirection::ClientToServer,
                PetSkillUseRequestFields.data(),
                PetSkillUseRequestFields.size(),
            },
            {
                "kjSK",
                PacketDirection::ClientToServer,
                PetLevelUpPointFields.data(),
                PetLevelUpPointFields.size(),
            },
            {
                "HJQp",
                PacketDirection::ClientToServer,
                PetSkillSwapFields.data(),
                PetSkillSwapFields.size(),
            },
            {
                "ycX",
                PacketDirection::ClientToServer,
                UserPetNameInputFields.data(),
                UserPetNameInputFields.size(),
            },
            {
                "CV",
                PacketDirection::ClientToServer,
                MailMessageSendFields.data(),
                MailMessageSendFields.size(),
            },
            {
                "uSr",
                PacketDirection::ClientToServer,
                ChatMessageSendFields.data(),
                ChatMessageSendFields.size(),
            },
            {
                "xD",
                PacketDirection::ClientToServer,
                WindowOpenSendFields.data(),
                WindowOpenSendFields.size(),
            },
            {
                "IcE",
                PacketDirection::ClientToServer,
                ShutdownRequestFields.data(),
                ShutdownRequestFields.size(),
            },
            {
                "rMsv",
                PacketDirection::ClientToServer,
                PetMailSendFields.data(),
                PetMailSendFields.size(),
            },
            {
                "PfqV",
                PacketDirection::ClientToServer,
                PetMailRegisterFields.data(),
                PetMailRegisterFields.size(),
            },
            {
                "cNB",
                PacketDirection::ServerToClient,
                ProfileMailDeleteFields.data(),
                ProfileMailDeleteFields.size(),
            },
            {
                "ow",
                PacketDirection::ServerToClient,
                ProfileMailReceiptFields.data(),
                ProfileMailReceiptFields.size(),
            },
            {
                "KyN",
                PacketDirection::ServerToClient,
                ProfileMailArriveFields.data(),
                ProfileMailArriveFields.size(),
            },
            {
                "TNT",
                PacketDirection::ServerToClient,
                PartyParamFields.data(),
                PartyParamFields.size(),
            },
            {
                "JJQj",
                PacketDirection::ServerToClient,
                PartyMemberStatusFields.data(),
                PartyMemberStatusFields.size(),
            },
            {
                "Rspn",
                PacketDirection::ServerToClient,
                TechListFields.data(),
                TechListFields.size(),
            },
            {
                "OBee",
                PacketDirection::ServerToClient,
                PetTechDataFields.data(),
                PetTechDataFields.size(),
            },
            {
                "IsSy",
                PacketDirection::ServerToClient,
                RidePetDataFields.data(),
                RidePetDataFields.size(),
            },
            {
                "AU",
                PacketDirection::ServerToClient,
                RidePetDataFields.data(),
                RidePetDataFields.size(),
            },
            {
                "JgO",
                PacketDirection::ServerToClient,
                TechUseResultFields.data(),
                TechUseResultFields.size(),
            },
            {
                "Yo",
                PacketDirection::ServerToClient,
                ProfileStatusFields.data(),
                ProfileStatusFields.size(),
            },
            {
                "ysVc",
                PacketDirection::ServerToClient,
                FrontCharListFields.data(),
                FrontCharListFields.size(),
            },
            {
                "LY",
                PacketDirection::ServerToClient,
                FrontCharListFields.data(),
                FrontCharListFields.size(),
            },
            {
                "ND",
                PacketDirection::ServerToClient,
                CharacterPetListFields.data(),
                CharacterPetListFields.size(),
            },
            {
                "nFV",
                PacketDirection::ServerToClient,
                CharacterPetListFields.data(),
                CharacterPetListFields.size(),
            },
            ProfileWindowSpec,
            {
                "Zd",
                PacketDirection::ServerToClient,
                SystemMessageFields.data(),
                SystemMessageFields.size(),
            },
            {
                "zzd",
                PacketDirection::ServerToClient,
                EffectFields.data(),
                EffectFields.size(),
            },
            BattleRewardSpec,
            {
                "hB",
                PacketDirection::ServerToClient,
                DuelPointFields.data(),
                DuelPointFields.size(),
            },
            {
                "LYIm",
                PacketDirection::ServerToClient,
                BattleCommandFields.data(),
                BattleCommandFields.size(),
            },
            {
                "Mhbt",
                PacketDirection::ServerToClient,
                ProfileMailMessageFields.data(),
                ProfileMailMessageFields.size(),
            },
            ChatMessageSpec,
            {
                "BZ",
                PacketDirection::ServerToClient,
                WindowPacketFields.data(),
                WindowPacketFields.size(),
            },
            {
                "ft",
                PacketDirection::ServerToClient,
                StallBrowseFields.data(),
                StallBrowseFields.size(),
            },
            {
                "ylGN",
                PacketDirection::ClientToServer,
                StallStartFields.data(),
                StallStartFields.size(),
            },
            {
                "CCL",
                PacketDirection::ClientToServer,
                CheckCommandFields.data(),
                CheckCommandFields.size(),
            },
            {
                "TeD",
                PacketDirection::ServerToClient,
                MapNameFields.data(),
                MapNameFields.size(),
            },
            AddressBookSpec,
            AddressBookGuildInfoSpec,
            AddressBookInfoSpec,
            GuildInfoSpec,
            GuildTitleSpec,
            GuildMemberSpec,
            GuildMemberInfoSpec,
            GuildMailSpec,
            TradePlayerListSpec,
            TradeStartSpec,
            TradeItemInfoSpec,
            TradePetInfoSpec,
            TradePetSkillInfoSpec,
            {
                "aiAb",
                PacketDirection::ClientToServer,
                TradeTakeOutFields.data(),
                TradeTakeOutFields.size(),
            },
            {
                "woz",
                PacketDirection::ClientToServer,
                DropGoldFields.data(),
                DropGoldFields.size(),
            },
            {
                "Ufm",
                PacketDirection::ClientToServer,
                GuildInfoChangeFields.data(),
                GuildInfoChangeFields.size(),
            },
            {
                "GY",
                PacketDirection::ClientToServer,
                GuildTitleSetFields.data(),
                GuildTitleSetFields.size(),
            },
            {
                "uId",
                PacketDirection::ClientToServer,
                GuildMailSendFields.data(),
                GuildMailSendFields.size(),
            },
            {
                "wD",
                PacketDirection::ClientToServer,
                ProfileMailFriendDeleteFields.data(),
                ProfileMailFriendDeleteFields.size(),
            },
            ProfileMiniMailSendSpec,
            {
                "nsY",
                PacketDirection::ServerToClient,
                ObjectListFields.data(),
                ObjectListFields.size(),
            },
            {
                "NI",
                PacketDirection::ServerToClient,
                CharacterIconFields.data(),
                CharacterIconFields.size(),
            },
            {
                "aBS",
                PacketDirection::ServerToClient,
                MapDataFields.data(),
                MapDataFields.size(),
            },
            {
                "UUN",
                PacketDirection::ClientToServer,
                MapDataRequestFields.data(),
                MapDataRequestFields.size(),
            },
            {
                "uSB",
                PacketDirection::ServerToClient,
                ObjectDeleteFields.data(),
                ObjectDeleteFields.size(),
            },
            {
                "Gp",
                PacketDirection::ClientToServer,
                nullptr,
                0,
            },
        }};

        [[nodiscard]] inline const PacketSpec* FindPacketSpec(std::string_view code) noexcept
        {
            for (const PacketSpec& spec : PacketRegistry) {
                if (spec.code == code) {
                    return &spec;
                }
            }
            return nullptr;
        }

        [[nodiscard]] inline PacketError MakeParserError(
            PacketErrorCode errorCode,
            std::string message)
        {
            return PacketError::Make(errorCode, std::move(message));
        }

        inline void FillErrorContext(PacketError& error, std::string_view code, std::size_t fieldIndex)
        {
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldIndex;
        }

        [[nodiscard]] inline PacketResult<std::string> DecodeNestedPayloadEscapedString(std::string_view text)
        {
            std::string output;
            output.reserve(text.size());

            for (std::size_t index = 0; index < text.size(); ++index) {
                const char ch = text[index];
                if (IsLegacyDoubleByteLead(ch)) {
                    if (index + 1U >= text.size()) {
                        return PacketResult<std::string>::Failure(MakeEscapedStringError(
                            PacketErrorCode::DecodeFailed,
                            "es 字符串旧双字节字符被截断"));
                    }

                    output.push_back(ch);
                    output.push_back(text[index + 1U]);
                    ++index;
                    continue;
                }

                if (ch != '\\') {
                    output.push_back(ch);
                    continue;
                }

                if (index + 1U >= text.size()) {
                    output.push_back('\\');
                    continue;
                }

                const char escaped = text[index + 1U];
                if (escaped == 'S') {
                    output.push_back(' ');
                } else if (escaped == '\\') {
                    output.push_back('\\');
                } else {
                    output.push_back('\\');
                    output.push_back(escaped);
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::move(output));
        }
    }

    /**
     * 解析 DecodeEnvelope 后的明文 token 串；不接受 envelope 文本。
     */
    [[nodiscard]] inline PacketResult<Packet> ParsePacket(std::string_view plainTokens)
    {
        PacketResult<std::vector<std::string>> splitResult = SplitPlainTokens(plainTokens);
        if (!splitResult.HasValue()) {
            return PacketResult<Packet>::Failure(splitResult.Error());
        }

        std::vector<std::string> tokens = splitResult.Value();
        if (tokens.empty()) {
            return PacketResult<Packet>::Failure(detail::MakeParserError(
                PacketErrorCode::EmptyInput,
                "封包明文 token 输入为空"));
        }

        const std::string& code = tokens.front();
        if (code.empty()) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, "封包 code 为空");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        const PacketSpec* spec = detail::FindPacketSpec(code);
        if (spec == nullptr) {
            PacketError error = detail::MakeParserError(PacketErrorCode::UnknownCode, "封包 code 未注册");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        std::vector<std::string> fieldTokens(tokens.begin() + 1, tokens.end());
        if (fieldTokens.size() == spec->fieldCount + 1U && !fieldTokens.empty() && fieldTokens.back().empty()) {
            fieldTokens.pop_back();
        }

        if (fieldTokens.size() != spec->fieldCount) {
            PacketError error = detail::MakeParserError(
                PacketErrorCode::FieldCountMismatch,
                "封包字段数量不匹配");
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldTokens.size() < spec->fieldCount ? fieldTokens.size() : spec->fieldCount;
            return PacketResult<Packet>::Failure(std::move(error));
        }

        Packet packet(PacketCode(code), spec->direction);
        packet.fields.reserve(spec->fieldCount);

        for (std::size_t index = 0; index < spec->fieldCount; ++index) {
            const PacketFieldSpec& fieldSpec = spec->fields[index];
            const std::string& token = fieldTokens[index];

            PacketField field;
            field.name = std::string(fieldSpec.name);
            field.type = fieldSpec.type;

            switch (fieldSpec.type) {
            case PacketFieldType::Int62: {
                PacketResult<int> decoded = DecodeInt62(token);
                if (!decoded.HasValue()) {
                    PacketError error = decoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<Packet>::Failure(std::move(error));
                }
                field.value = std::to_string(decoded.Value());
                break;
            }
            case PacketFieldType::EscapedString: {
                PacketResult<std::string> decoded = DecodeEscapedString(token);
                if (!decoded.HasValue()) {
                    PacketError error = decoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<Packet>::Failure(std::move(error));
                }
                field.value = decoded.Value();
                break;
            }
            case PacketFieldType::RawString:
                field.value = token;
                break;
            case PacketFieldType::Payload:
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    "封包字段类型暂不支持");
                detail::FillErrorContext(error, code, index);
                return PacketResult<Packet>::Failure(std::move(error));
            }
            }

            packet.fields.push_back(std::move(field));
        }

        return PacketResult<Packet>::Success(std::move(packet));
    }

    [[nodiscard]] inline PacketResult<Packet> ParsePacketWithSubPayloadFields(
        std::string_view plainTokens,
        const PacketSpec& packetSpec,
        std::span<const std::size_t> fieldIndexes)
    {
        PacketResult<Packet> parsed = ParsePacket(plainTokens);
        if (!parsed.HasValue()) {
            return parsed;
        }

        Packet packet = std::move(parsed.Value());
        if (packet.code.View() != packetSpec.code || fieldIndexes.empty()) {
            return PacketResult<Packet>::Success(std::move(packet));
        }

        PacketResult<std::vector<std::string>> splitResult = SplitPlainTokens(plainTokens);
        if (!splitResult.HasValue()) {
            return PacketResult<Packet>::Failure(splitResult.Error());
        }

        std::vector<std::string> tokens = std::move(splitResult.Value());
        std::vector<std::string> fieldTokens(tokens.begin() + 1, tokens.end());
        if (fieldTokens.size() == packet.fields.size() + 1U && !fieldTokens.empty() && fieldTokens.back().empty()) {
            fieldTokens.pop_back();
        }

        for (const std::size_t fieldIndex : fieldIndexes) {
            if (fieldIndex >= packetSpec.fieldCount || fieldIndex >= packet.fields.size() ||
                fieldIndex >= fieldTokens.size()) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::FieldCountMismatch,
                    "sub payload 字段索引越界");
                error.packetCode = packet.code;
                error.fieldIndex = fieldIndex;
                return PacketResult<Packet>::Failure(std::move(error));
            }

            const PacketFieldSpec& fieldSpec = packetSpec.fields[fieldIndex];
            if (packet.fields[fieldIndex].name != fieldSpec.name) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::DecodeFailed,
                    "sub payload 字段名不匹配");
                detail::FillErrorContext(error, packet.code.View(), fieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }

            if (fieldSpec.type != PacketFieldType::EscapedString ||
                packet.fields[fieldIndex].type != PacketFieldType::EscapedString) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    "sub payload 字段类型不匹配");
                detail::FillErrorContext(error, packet.code.View(), fieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }

            PacketResult<std::string> decoded =
                detail::DecodeNestedPayloadEscapedString(fieldTokens[fieldIndex]);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                detail::FillErrorContext(error, packet.code.View(), fieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }

            packet.fields[fieldIndex].value = std::move(decoded.Value());
        }

        return PacketResult<Packet>::Success(std::move(packet));
    }

    [[nodiscard]] inline PacketResult<Packet> ParsePacketWithSubPayloadField(
        std::string_view plainTokens,
        const PacketSpec& packetSpec,
        std::size_t fieldIndex)
    {
        const std::array<std::size_t, 1> fieldIndexes = {fieldIndex};
        return ParsePacketWithSubPayloadFields(plainTokens, packetSpec, fieldIndexes);
    }
}

#endif // CG_ENGINE_PACKET_PARSER_H
