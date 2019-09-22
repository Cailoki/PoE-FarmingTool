#include "pch.h"
#include "Models.h"

//Declaration of ItemMods class static members
const std::vector<std::wstring> ItemMods::mapPrefixModifiers = { L"Ceremonial", L"Antagonist's", L"Anarchic", L"Skeletal", L"Capricious", L"Slithering", L"Undead", L"Emanant", L"Feral", L"Demonic", L"Bipedal", L"Solar",
L"Lunar", L"Haunting", L"Feasting", L"Multifarious", L"Chaining", L"Otherworldly", L"Twinned", L"Splitting", L"Abhorrent", L"Unstoppable", L"Conflagrating", L"Unwavering", L"Armoured", L"Fecund", L"Savage",
L"Burning", L"Freezing", L"Shocking", L"Fleet", L"Punishing", L"Mirrored", L"Overlord's", L"Titan's", L"Hexwarded", L"Resistant", L"Impervious", L"Hexproof", L"Empowered" };

//Declaration of Maps class static members
//SPECIAL CASE: "Caer Blaidd, Wolfpack's Den" -> is without comma in downloaded data from api
const std::vector<Maps::UniqueMap> Maps::uniqueMaps = {
		{L"Acton's Nightmare", L"Overgrown Shrine Map"} , {L"Caer Blaidd Wolfpack's Den", L"Underground River Map"}, {L"Death and Taxes", L"Necropolis Map"}, {L"Doryani's Machinarium", L"Maze Map"},
		{L"Hall of Grandmasters", L"Promenade Map"}, {L"Hallowed Ground", L"Cemetery Map"}, {L"Maelström of Chaos", L"Atoll Map"}, {L"Mao Kun",L"Shore Map"}, {L"Oba's Cursed Trove", L"Underground Sea Map"},
		{L"Olmec's Sanctum",L"Bone Crypt Map"}, {L"Pillars of Arun", L"Dunes Map"}, {L"Poorjoy's Asylum", L"Temple Map"}, {L"The Beachhead", L"The Coward's Trial"}, {L"The Perandus Manor", L"Chateau Map"},
		{L"The Putrid Cloister", L"Museum Map"}, {L"The Twilight Temple", L"Moon Temple Map"}, {L"The Vinktar Square", L"Courtyard Map"}, {L"Vaults of Atziri", L"Vaal Pyramid Map"}, {L"Whakawairua Tuahu", L"Strand Map"}
};