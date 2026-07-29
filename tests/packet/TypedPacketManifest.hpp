#pragma once

#include "packet/Receive.hpp"
#include "packet/Send.hpp"

#include <array>
#include <concepts>
#include <string>
#include <string_view>

namespace cg::packet::test {

struct TypedPacketManifestEntry
{
    const PacketSpec* spec = nullptr;
};

template <typename PacketT, const PacketSpec& SpecValue>
consteval TypedPacketManifestEntry MakeSendManifestEntry()
{
    static_assert(requires(const PacketT& packet) {
        { packet.Encode() } -> std::same_as<PacketResult<std::string>>;
    });
    static_assert(&SpecValue == &PacketT::Spec);
    static_assert(PacketT::Spec.code == PacketT::Code);
    static_assert(PacketT::Spec.direction == PacketT::Direction);
    static_assert(PacketT::Spec.fields == PacketT::Fields.data());
    static_assert(PacketT::Spec.fieldCount == PacketT::Fields.size());
    return {&SpecValue};
}

template <typename PacketT, const PacketSpec& SpecValue>
consteval TypedPacketManifestEntry MakeReceiveManifestEntry()
{
    static_assert(requires(std::string_view view) {
        { PacketT::Parse(view) } -> std::same_as<PacketResult<PacketT>>;
    });
    static_assert(&SpecValue == &PacketT::Spec);
    static_assert(PacketT::Spec.code == PacketT::Code);
    static_assert(PacketT::Spec.direction == PacketT::Direction);
    static_assert(PacketT::Spec.fields == PacketT::Fields.data());
    static_assert(PacketT::Spec.fieldCount == PacketT::Fields.size());
    return {&SpecValue};
}

inline constexpr std::array<TypedPacketManifestEntry, 28U> SendTypedPackets = {{
    MakeSendManifestEntry<send::ClientLogin, send::ClientLogin::Spec>(),
    MakeSendManifestEntry<send::CharacterListRequest, send::CharacterListRequest::Spec>(),
    MakeSendManifestEntry<send::CharacterLoginRequest, send::CharacterLoginRequest::Spec>(),
    MakeSendManifestEntry<send::CharacterLogoutRequest, send::CharacterLogoutRequest::Spec>(),
    MakeSendManifestEntry<send::CheckCommand, send::CheckCommand::Spec>(),
    MakeSendManifestEntry<send::ProfileStatusUpdateRequest, send::ProfileStatusUpdateRequest::Spec>(),
    MakeSendManifestEntry<send::CreateNewChar, send::CreateNewChar::Spec>(),
    MakeSendManifestEntry<send::MapDataRequest, send::MapDataRequest::Spec>(),
    MakeSendManifestEntry<send::SetProfileData, send::SetProfileData::Spec>(),
    MakeSendManifestEntry<send::MagicUseRequest, send::MagicUseRequest::Spec>(),
    MakeSendManifestEntry<send::PetSkillUseRequest, send::PetSkillUseRequest::Spec>(),
    MakeSendManifestEntry<send::PetLevelUpPoint, send::PetLevelUpPoint::Spec>(),
    MakeSendManifestEntry<send::PetSkillSwap, send::PetSkillSwap::Spec>(),
    MakeSendManifestEntry<send::UserPetNameInput, send::UserPetNameInput::Spec>(),
    MakeSendManifestEntry<send::MailMessageSend, send::MailMessageSend::Spec>(),
    MakeSendManifestEntry<send::ChatMessageSend, send::ChatMessageSend::Spec>(),
    MakeSendManifestEntry<send::WindowOpenSend, send::WindowOpenSend::Spec>(),
    MakeSendManifestEntry<send::ShutdownRequest, send::ShutdownRequest::Spec>(),
    MakeSendManifestEntry<send::PetMailSend, send::PetMailSend::Spec>(),
    MakeSendManifestEntry<send::PetMailRegister, send::PetMailRegister::Spec>(),
    MakeSendManifestEntry<send::DropGold, send::DropGold::Spec>(),
    MakeSendManifestEntry<send::GuildInfoChange, send::GuildInfoChange::Spec>(),
    MakeSendManifestEntry<send::GuildTitleSet, send::GuildTitleSet::Spec>(),
    MakeSendManifestEntry<send::GuildMailSend, send::GuildMailSend::Spec>(),
    MakeSendManifestEntry<send::ProfileMailFriendDelete, send::ProfileMailFriendDelete::Spec>(),
    MakeSendManifestEntry<send::ProfileMiniMailSend, send::ProfileMiniMailSend::Spec>(),
    MakeSendManifestEntry<send::TradeTakeOut, send::TradeTakeOut::Spec>(),
    MakeSendManifestEntry<send::StallStart, send::StallStart::Spec>(),
}};

inline constexpr std::array<TypedPacketManifestEntry, 67U> ReceiveTypedPackets = {{
    MakeReceiveManifestEntry<receive::ClientLoginResult, receive::ClientLoginResult::Spec>(),
    MakeReceiveManifestEntry<receive::PfMNotification, receive::PfMNotification::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterList, receive::CharacterList::Spec>(),
    MakeReceiveManifestEntry<receive::CreateNewCharResult, receive::CreateNewCharResult::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterDeleteResult, receive::CharacterDeleteResult::Spec>(),
    MakeReceiveManifestEntry<receive::ProcessStatus, receive::ProcessStatus::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileVersion, receive::ProfileVersion::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterProfile, receive::CharacterProfile::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterProfile2, receive::CharacterProfile2::Spec>(),
    MakeReceiveManifestEntry<receive::TitleList, receive::TitleList::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileList, receive::ProfileList::Spec>(),
    MakeReceiveManifestEntry<receive::AboutList, receive::AboutList::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileData, receive::ProfileData::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileMailDelete, receive::ProfileMailDelete::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileMailReceipt, receive::ProfileMailReceipt::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileMailArrive, receive::ProfileMailArrive::Spec>(),
    MakeReceiveManifestEntry<receive::PartyParam, receive::PartyParam::Spec>(),
    MakeReceiveManifestEntry<receive::PartyMemberStatus, receive::PartyMemberStatus::Spec>(),
    MakeReceiveManifestEntry<receive::TechList, receive::TechList::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterSkillData, receive::CharacterSkillData::Spec>(),
    MakeReceiveManifestEntry<receive::ItemData, receive::ItemData::Spec>(),
    MakeReceiveManifestEntry<receive::PetItemData, receive::PetItemData::Spec>(),
    MakeReceiveManifestEntry<receive::EquipSuit, receive::EquipSuit::Spec>(),
    MakeReceiveManifestEntry<receive::ItemInfoList, receive::ItemInfoList::Spec>(),
    MakeReceiveManifestEntry<receive::ItemRecipe, receive::ItemRecipe::Spec>(),
    MakeReceiveManifestEntry<receive::AlbumFlags, receive::AlbumFlags::Spec>(),
    MakeReceiveManifestEntry<receive::AlbumInfo, receive::AlbumInfo::Spec>(),
    MakeReceiveManifestEntry<receive::AlbumName, receive::AlbumName::Spec>(),
    MakeReceiveManifestEntry<receive::PetMailAnimation, receive::PetMailAnimation::Spec>(),
    MakeReceiveManifestEntry<receive::RidePetData, receive::RidePetData::Spec>(),
    MakeReceiveManifestEntry<receive::RidePetExtraData, receive::RidePetExtraData::Spec>(),
    MakeReceiveManifestEntry<receive::TechUseResult, receive::TechUseResult::Spec>(),
    MakeReceiveManifestEntry<receive::PetTechData, receive::PetTechData::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileStatus, receive::ProfileStatus::Spec>(),
    MakeReceiveManifestEntry<receive::FrontCharList, receive::FrontCharList::Spec>(),
    MakeReceiveManifestEntry<receive::ItemFrontCharList, receive::ItemFrontCharList::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterPetList, receive::CharacterPetList::Spec>(),
    MakeReceiveManifestEntry<receive::ItemCharacterPetList, receive::ItemCharacterPetList::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileWindow, receive::ProfileWindow::Spec>(),
    MakeReceiveManifestEntry<receive::SystemMessage, receive::SystemMessage::Spec>(),
    MakeReceiveManifestEntry<receive::Effect, receive::Effect::Spec>(),
    MakeReceiveManifestEntry<receive::BattleReward, receive::BattleReward::Spec>(),
    MakeReceiveManifestEntry<receive::DuelPoint, receive::DuelPoint::Spec>(),
    MakeReceiveManifestEntry<receive::BattleCommand, receive::BattleCommand::Spec>(),
    MakeReceiveManifestEntry<receive::ProfileMailMessage, receive::ProfileMailMessage::Spec>(),
    MakeReceiveManifestEntry<receive::ChatMessage, receive::ChatMessage::Spec>(),
    MakeReceiveManifestEntry<receive::WindowPacket, receive::WindowPacket::Spec>(),
    MakeReceiveManifestEntry<receive::StallBrowse, receive::StallBrowse::Spec>(),
    MakeReceiveManifestEntry<receive::JobName, receive::JobName::Spec>(),
    MakeReceiveManifestEntry<receive::MapName, receive::MapName::Spec>(),
    MakeReceiveManifestEntry<receive::AddressBook, receive::AddressBook::Spec>(),
    MakeReceiveManifestEntry<receive::AddressBookInfo, receive::AddressBookInfo::Spec>(),
    MakeReceiveManifestEntry<receive::GuildInfo, receive::GuildInfo::Spec>(),
    MakeReceiveManifestEntry<receive::GuildTitle, receive::GuildTitle::Spec>(),
    MakeReceiveManifestEntry<receive::GuildMember, receive::GuildMember::Spec>(),
    MakeReceiveManifestEntry<receive::GuildMemberInfo, receive::GuildMemberInfo::Spec>(),
    MakeReceiveManifestEntry<receive::GuildMail, receive::GuildMail::Spec>(),
    MakeReceiveManifestEntry<receive::TradePlayerList, receive::TradePlayerList::Spec>(),
    MakeReceiveManifestEntry<receive::TradeStart, receive::TradeStart::Spec>(),
    MakeReceiveManifestEntry<receive::TradeItemInfo, receive::TradeItemInfo::Spec>(),
    MakeReceiveManifestEntry<receive::TradePetInfo, receive::TradePetInfo::Spec>(),
    MakeReceiveManifestEntry<receive::TradePetSkillInfo, receive::TradePetSkillInfo::Spec>(),
    MakeReceiveManifestEntry<receive::AddressBookGuildInfo, receive::AddressBookGuildInfo::Spec>(),
    MakeReceiveManifestEntry<receive::ObjectList, receive::ObjectList::Spec>(),
    MakeReceiveManifestEntry<receive::CharacterIcon, receive::CharacterIcon::Spec>(),
    MakeReceiveManifestEntry<receive::ObjectDelete, receive::ObjectDelete::Spec>(),
    MakeReceiveManifestEntry<receive::MapData, receive::MapData::Spec>(),
}};

} // namespace cg::packet::test
