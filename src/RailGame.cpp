#include "RailGame.hpp"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <vector>

namespace Railfinity {

namespace {
#ifdef __ANDROID__
constexpr const char* SAVE_FILE_NAME = "/data/data/com.sebinylabs.railfinity/files/railfinity_save.dat";
#else
constexpr const char* SAVE_FILE_NAME = "railfinity_save.dat";
#endif

const std::array<WeaponDef, NUM_WEAPONS> WEAPON_CATALOG = {{
    { 0, "OMNIWRENCH 8000", "OMNIWRENCH MILLENNIUM 12",
      "SPECIAL: Heavy Melee Strike + Deflects Enemy Shots & +2 Bonus Crate Bolts",
      "Ratchet's signature melee weapon! Swings in front to smash chests, drones & Enforcers up close.",
      0, 2, 0.26f, 35.0f, { 85, 225, 255, 255 } },
    { 1, "MAGMA COMBUSTER", "MAGMA CANNON V5",
      "SPECIAL: Enforcer Shield Melter (2.5x Dmg) + Twin/Triple Plasma Stream",
      "Rapid-fire plasma blaster that melts through Enforcer shields and fires multi-bolt magma streams.",
      400, 3, 0.16f, 18.0f, { 255, 130, 30, 255 } },
    { 2, "CONSTRUCTO SHOTGUN", "CONSTRUCTO SHREDDER V5",
      "SPECIAL: 3-Rail Shockwave Blast + Vaporizes Enemy Plasma Shots",
      "Fires a wide kinetic shockwave across all 3 rails at once and vaporizes incoming enemy shots!",
      900, 4, 0.40f, 26.0f, { 80, 220, 255, 255 } },
    { 3, "BUZZ BLADES", "DOOM BLADES V5",
      "SPECIAL: Smart Cross-Rail Pinball Ricochet (Seeks Targets Across All 3 Rails)",
      "Launches high-RPM saw discs that automatically seek and bounce between targets across all 3 rails!",
      1600, 5, 0.23f, 24.0f, { 120, 255, 110, 255 } },
    { 4, "PLASMA STRIKER", "SUPERMASSIVE STRIKER V5",
      "SPECIAL: Infinite Rail Pierce + EMP Chain-Lightning To Adjacent Rails",
      "Instant railgun hyperbeam that pierces every target in your lane and arcs EMP lightning sideways!",
      2600, 6, 0.48f, 58.0f, { 210, 90, 255, 255 } },
    { 5, "RYNO INFINITE", "RYNO V5 OMEGA",
      "SPECIAL: All-Rail Homing Missile Swarm + Explosive AoE & +50% Bolt Jackpot",
      "Rip Ya A New One! Homing micro-missiles lock onto all 3 rails with AoE blasts and +50% bonus Bolts!",
      5000, 10, 0.28f, 45.0f, { 255, 55, 65, 255 } }
}};

const std::array<SkillPointDef, NUM_SKILL_POINTS> SKILL_POINT_CATALOG = {{
    { 0, "GRIND BOOT INITIATE", "Survive 500m on the intergalactic grindrails in a single run.", 250 },
    { 1, "WRENCH NINJA", "Smash 25 Crates using your OmniWrench or Weapons in one run.", 350 },
    { 2, "MAXIMUM MULTIPLIER", "Reach the 20x Bolt Multiplier tier without taking damage!", 750 },
    { 3, "EXPLOSIVE DEMOLITION", "Detonate a TNT Barrel to blow up nearby hazards.", 300 },
    { 4, "THUG BUSTER", "Destroy 15 Thugs-4-Less Drones or Enforcers in a single run.", 500 },
    { 5, "OMEGA ARSENAL", "Upgrade any Gadgetron weapon to Level V5 OMEGA.", 1000 },
    { 6, "PROG TWIN TAKEDOWN", "Defeat the Command Warship Boss encounter on the rails.", 800 },
    { 7, "DIMENSION HOPPER", "Reach Sector 4: The Dimensionator Rift (1,500m+).", 600 },
    { 8, "RARITANIUM MINER", "Harvest a crystalline Raritanium Cache on the rails.", 400 },
    { 9, "LOCKED AND LOADED", "Unlock at least 3 different Gadgetron weapons.", 700 }
}};

const std::array<DailyMutatorDef, NUM_DAILY_MUTATORS> MUTATOR_CATALOG = {{
    { DailyMutatorId::ARMORED_CONVOY, "ARMORED CONVOY",
      "HAZARD: Heavy Thug Drone & Shielded Enforcer patrols on the rails.",
      "PERK: 2x OmniWrench & Combuster Damage + +50% Crate Ammo Restock." },
    { DailyMutatorId::HIGH_VOLTAGE_GRID, "HIGH-VOLTAGE GRID",
      "HAZARD: Dense Electrified Rail coils & security Laser Gates.",
      "PERK: Cross-Rail Attractor pulls Bolts & Raritanium from adjacent lanes." },
    { DailyMutatorId::VOLATILE_CARGO, "VOLATILE CARGO",
      "HAZARD: Unstable Megacorp TNT Barrels scattered across corridors.",
      "PERK: TNT blasts chain 16m across all 3 rails & double Crate Bolt drops." },
    { DailyMutatorId::OVERCLOCKED_RAILS, "OVERCLOCKED RAILS",
      "HAZARD: +12% Overclocked Grind Speed & frequent Hyper-Boost ramps.",
      "PERK: Bolt Multiplier escalates 1.6x faster toward 20x." },
    { DailyMutatorId::AERIAL_ASSAULT, "AERIAL ASSAULT",
      "HAZARD: Frequent Low Laser curtains & high-altitude rail jumps.",
      "PERK: 2x Clank Heli-Pack fuel capacity + mid-air thruster trickle recharge." },
    { DailyMutatorId::GADGETRON_OVERLOAD, "GADGETRON OVERLOAD",
      "HAZARD: Boss Warships & Dread-Mechs deploy +35% reinforced armor.",
      "PERK: +30% faster Weapon Fire Rate & +50% Max Firearm Ammo reserves." },
    { DailyMutatorId::RARITANIUM_RUSH, "RARITANIUM RUSH",
      "HAZARD: Cross-sector Hover-Taxis, Magma Geysers, Piranhas & Cranes.",
      "PERK: +32% of standard Bolt Crates crystallize into Raritanium Caches!" },
    { DailyMutatorId::DEADLOCKED_GAUNTLET, "DEADLOCKED GAUNTLET",
      "HAZARD: Boss Warships & Dread-Mechs ambush every 500m instead of 750m.",
      "PERK: Defeating any Boss repairs +2 Nanotech HP & drops +3 extra Raritanium." }
}};

constexpr size_t CONTRACT_POOL_SIZE = 12;
const std::array<DailyContractDef, CONTRACT_POOL_SIZE> CONTRACT_POOL = {{
    { 0,  "CRATE BREAKER",      "Smash 35 Gadgetron Crates or Caches on the rails.", 35, 350, 3 },
    { 1,  "WRENCH BRAWLER",     "Smash 18 targets up close using your OmniWrench.", 18, 350, 3 },
    { 2,  "THUG HUNTER",        "Destroy 16 Thug Drones, Enforcers, or Hover-Taxis.", 16, 400, 3 },
    { 3,  "VERSA-FUSE ACROBAT", "Grapple 5 green Swingshot Nodes across rail chasms.", 5, 350, 3 },
    { 4,  "DEMOLITION EXPERT",  "Detonate 4 explosive TNT Barrels.", 4, 350, 3 },
    { 5,  "HIGH ROLLER",        "Reach a 12x (or higher) Bolt Multiplier.", 1, 400, 4 },
    { 6,  "MARATHON GRINDER",   "Grind 1,500 cumulative meters across runs.", 1500, 350, 3 },
    { 7,  "CRYSTAL PROSPECTOR", "Mine 6 crystalline Raritanium Ores.", 6, 400, 3 },
    { 8,  "BOSS SLAYER",        "Defeat a Boss Warship, Frigate, Galleon, or Mech.", 1, 450, 4 },
    { 9,  "DEFLECTOR SHIELD",   "Deflect or vaporize 5 incoming enemy plasma shots.", 5, 350, 3 },
    { 10, "SUPERSONIC RIDER",   "Trigger 6 Hyper-Boost Pads or Overdrive rushes.", 6, 350, 3 },
    { 11, "SECTOR EXPLORER",    "Clear 3 galactic Sector Warp Gate transitions.", 3, 400, 3 }
}};
} // namespace

RailGame::RailGame() = default;
RailGame::~RailGame() = default;

const std::array<WeaponDef, NUM_WEAPONS>& RailGame::GetWeaponCatalog() {
    return WEAPON_CATALOG;
}

const std::array<SkillPointDef, NUM_SKILL_POINTS>& RailGame::GetSkillPointCatalog() {
    return SKILL_POINT_CATALOG;
}

const std::array<DailyMutatorDef, NUM_DAILY_MUTATORS>& RailGame::GetMutatorCatalog() {
    return MUTATOR_CATALOG;
}

const DailyContractDef& RailGame::GetContractDef(int contractId) {
    int clamped = std::clamp(contractId, 0, static_cast<int>(CONTRACT_POOL_SIZE) - 1);
    return CONTRACT_POOL[clamped];
}

DailyMutatorId RailGame::GetTodaysMutator() const {
    std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);
    int dayKey = (lt->tm_year + 1900) * 372 + (lt->tm_mon + 1) * 31 + lt->tm_mday;
    return static_cast<DailyMutatorId>(std::abs(dayKey) % static_cast<int>(NUM_DAILY_MUTATORS));
}

bool RailGame::IsMutatorActive(DailyMutatorId mut) const {
    return isDailyRun_ && (activeMutator_ == mut);
}

void RailGame::UpdateVirtualResolution() {
    float sw = static_cast<float>(GetRenderWidth());
    float sh = static_cast<float>(GetRenderHeight());
    if (sw <= 0.0f || sh <= 0.0f) {
        sw = static_cast<float>(GetScreenWidth());
        sh = static_cast<float>(GetScreenHeight());
    }
    if (sw > 0.0f && sh > 0.0f) {
        virtualWidth_ = std::max(1280.0f, std::round(720.0f * (sw / sh)));
        uiScale_ = std::min(sw / virtualWidth_, sh / 720.0f);
        uiOffsetX_ = (sw - virtualWidth_ * uiScale_) * 0.5f;
        uiOffsetY_ = (sh - 720.0f * uiScale_) * 0.5f;
    } else {
        virtualWidth_ = 1280.0f;
        uiScale_ = 1.0f;
        uiOffsetX_ = 0.0f;
        uiOffsetY_ = 0.0f;
    }
    RefreshUiFonts(uiScale_);
}

void RailGame::Init() {
    UpdateVirtualResolution();
    InitUiFonts();

    audio_.Init();
    LoadSave();
    audio_.SetMasterVolume(save_.masterVolume);
    audio_.SetMusicVolume(save_.musicVolume);
    audio_.SetSfxVolume(save_.sfxVolume);
    if (save_.targetFps > 0) {
        SetTargetFPS(save_.targetFps);
    }

    // Configure 3D Over-the-Shoulder Camera (classic Ratchet & Clank low behind-the-back angle)
    camera_.position = { 0.0f, 3.25f, -6.8f };
    camera_.target = { 0.0f, 2.2f, 26.0f };
    camera_.up = { 0.0f, 1.0f, 0.0f };
    camera_.fovy = 52.0f;
    camera_.projection = CAMERA_PERSPECTIVE;

    // Seed background skyline buildings
    PCG32 bgRng(0x987654321ULL);
    for (size_t i = 0; i < MAX_SKY_BUILDINGS; ++i) {
        auto& b = buildings_[i];
        bool leftSide = (i % 2 == 0);
        float sideX = leftSide ? bgRng.RangeFloat(-58.0f, -16.0f) : bgRng.RangeFloat(16.0f, 58.0f);
        float z = static_cast<float>(i / 2) * 14.0f;
        float w = bgRng.RangeFloat(6.0f, 14.0f);
        float h = bgRng.RangeFloat(22.0f, 68.0f);
        float d = bgRng.RangeFloat(6.0f, 12.0f);
        b.pos = { sideX, h * 0.5f - 18.0f, z };
        b.size = { w, h, d };
        unsigned char shade = static_cast<unsigned char>(bgRng.RangeInt(26, 52));
        b.bodyColor = { shade, static_cast<unsigned char>(shade + 8), static_cast<unsigned char>(shade + 22), 255 };
        b.neonColor = (i % 3 == 0) ? Color{ 255, 155, 40, 255 } :
                      (i % 3 == 1) ? Color{ 60, 215, 255, 255 } : Color{ 210, 85, 255, 255 };
    }

    // Load Sebiny Labs Branding Splash Logo
    auto TryLoadTex = [](const std::initializer_list<const char*>& paths) -> Texture2D {
        for (const char* p : paths) {
#if !defined(PLATFORM_ANDROID)
            if (!FileExists(p)) continue;
#endif
            Texture2D tex = LoadTexture(p);
            if (tex.id != 0) return tex;
        }
        return Texture2D{};
    };

    splashLogo_ = TryLoadTex({ "assets/sebiny_splash_logo.png", "sebiny_splash_logo.png", "../assets/sebiny_splash_logo.png" });

    // Load Authentic 3D Ratchet Head Model (PS3 A Crack in Time)
    const char* modelCandidates[] = {
        "assets/models/ratchet/ratchet_head.obj",
        "models/ratchet/ratchet_head.obj",
        "../assets/models/ratchet/ratchet_head.obj",
        "assets/models/ratchet/Armor0.obj",
        "models/ratchet/Armor0.obj",
        "../assets/models/ratchet/Armor0.obj"
    };
    for (const char* path : modelCandidates) {
#if !defined(PLATFORM_ANDROID)
        if (!FileExists(path)) continue;
#endif
        Model m = LoadModel(path);
        if (m.meshCount > 0) {
            ratchetModel_ = m;
            ratchetModelLoaded_ = true;
            TraceLog(LOG_INFO, "RATCHET 3D MODEL: Successfully loaded (%d meshes, %d materials) from %s",
                     ratchetModel_.meshCount, ratchetModel_.materialCount, path);
            // On Android, LoadOBJ cannot chdir into APK assets, so tinyobj skips .mtl files and collapses
            // all materials into 1 untextured material. Reconstruct the exact 5 materials & submesh bindings!
            if (ratchetModel_.materialCount < 5) {
                Material* expanded = static_cast<Material*>(RL_CALLOC(5, sizeof(Material)));
                for (int mm = 0; mm < 5; ++mm) {
                    expanded[mm] = LoadMaterialDefault();
                }
                if (ratchetModel_.materials != nullptr) {
                    RL_FREE(ratchetModel_.materials);
                }
                ratchetModel_.materials = expanded;
                ratchetModel_.materialCount = 5;
            }
            constexpr int submeshMats[6] = { 0, 1, 2, 3, 4, 2 };
            for (int sm = 0; sm < ratchetModel_.meshCount && sm < 6; ++sm) {
                ratchetModel_.meshMaterial[sm] = submeshMats[sm];
            }
            defaultModelTextures_.resize(ratchetModel_.materialCount);
            for (int i = 0; i < ratchetModel_.materialCount; ++i) {
                defaultModelTextures_[i] = ratchetModel_.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture;
                ratchetModel_.materials[i].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
            }
            break;
        }
    }

    // Explicitly load Ratchet Default Textures identically to Rivet (guarantees APK AAssetManager load on Android!)
    ratchetHead2Diff_ = TryLoadTex({ "assets/models/ratchet/RatHead2Diff.png",    "models/ratchet/RatHead2Diff.png",    "../assets/models/ratchet/RatHead2Diff.png" });
    ratchetFur2_      = TryLoadTex({ "assets/models/ratchet/RatFur2.png",         "models/ratchet/RatFur2.png",         "../assets/models/ratchet/RatFur2.png" });
    ratchetFur_       = TryLoadTex({ "assets/models/ratchet/RatFur.png",          "models/ratchet/RatFur.png",          "../assets/models/ratchet/RatFur.png" });
    ratchetEyeDiff_   = TryLoadTex({ "assets/models/ratchet/EyeDiff.png",         "models/ratchet/EyeDiff.png",         "../assets/models/ratchet/EyeDiff.png" });
    ratchetHeadDiff_  = TryLoadTex({ "assets/models/ratchet/RatchetHeadDiff.png", "models/ratchet/RatchetHeadDiff.png", "../assets/models/ratchet/RatchetHeadDiff.png" });
    ratchetTexturesLoaded_ = (ratchetHead2Diff_.id != 0 || ratchetHeadDiff_.id != 0);
    if (ratchetModelLoaded_ && ratchetModel_.materialCount >= 5) {
        if (ratchetHead2Diff_.id != 0) ratchetModel_.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ratchetHead2Diff_;
        if (ratchetFur2_.id != 0)      ratchetModel_.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture = ratchetFur2_;
        if (ratchetFur_.id != 0)       ratchetModel_.materials[2].maps[MATERIAL_MAP_DIFFUSE].texture = ratchetFur_;
        if (ratchetEyeDiff_.id != 0)   ratchetModel_.materials[3].maps[MATERIAL_MAP_DIFFUSE].texture = ratchetEyeDiff_;
        if (ratchetHeadDiff_.id != 0)  ratchetModel_.materials[4].maps[MATERIAL_MAP_DIFFUSE].texture = ratchetHeadDiff_;
        for (int i = 0; i < 5 && i < static_cast<int>(defaultModelTextures_.size()); ++i) {
            defaultModelTextures_[i] = ratchetModel_.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture;
        }
    }

    // Load Rivet Alternate Dimensional Lombax Textures
    rivetHeadDiff_  = TryLoadTex({ "assets/models/ratchet/RivetHeadDiff.png",  "models/ratchet/RivetHeadDiff.png",  "../assets/models/ratchet/RivetHeadDiff.png" });
    rivetHead2Diff_ = TryLoadTex({ "assets/models/ratchet/RivetHead2Diff.png", "models/ratchet/RivetHead2Diff.png", "../assets/models/ratchet/RivetHead2Diff.png" });
    rivetFur_       = TryLoadTex({ "assets/models/ratchet/RivetFur.png",       "models/ratchet/RivetFur.png",       "../assets/models/ratchet/RivetFur.png" });
    rivetFur2_      = TryLoadTex({ "assets/models/ratchet/RivetFur2.png",      "models/ratchet/RivetFur2.png",      "../assets/models/ratchet/RivetFur2.png" });
    rivetEyeDiff_   = TryLoadTex({ "assets/models/ratchet/RivetEyeDiff.png",   "models/ratchet/RivetEyeDiff.png",   "../assets/models/ratchet/RivetEyeDiff.png" });
    rivetTexturesLoaded_ = (rivetHeadDiff_.id != 0);

#if defined(PLATFORM_ANDROID)
    // OpenGL ES 2.0 default Raylib shader uses mediump float (10-bit half-float), causing severe vertex
    // Z-quantization, diagonal polygon slicing, and asset pop-out on Mali GPUs. Force 32-bit highp float!
    const char* vHighp =
        "#version 100\n"
        "precision highp float;\n"
        "attribute highp vec3 vertexPosition;\n"
        "attribute mediump vec2 vertexTexCoord;\n"
        "attribute mediump vec4 vertexColor;\n"
        "uniform highp mat4 mvp;\n"
        "varying mediump vec2 fragTexCoord;\n"
        "varying mediump vec4 fragColor;\n"
        "void main()\n"
        "{\n"
        "    fragTexCoord = vertexTexCoord;\n"
        "    fragColor = vertexColor;\n"
        "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
        "}\n";
    const char* fMedp =
        "#version 100\n"
        "precision mediump float;\n"
        "varying vec2 fragTexCoord;\n"
        "varying vec4 fragColor;\n"
        "uniform sampler2D texture0;\n"
        "uniform vec4 colDiffuse;\n"
        "void main()\n"
        "{\n"
        "    vec4 texelColor = texture2D(texture0, fragTexCoord);\n"
        "    if (texelColor.a < 0.05) discard;\n"
        "    gl_FragColor = texelColor * colDiffuse * fragColor;\n"
        "}\n";
    worldHighpShader_ = LoadShaderFromMemory(vHighp, fMedp);
    if (worldHighpShader_.id != 0) {
        worldHighpShaderLoaded_ = true;
        if (ratchetModelLoaded_) {
            for (int i = 0; i < ratchetModel_.materialCount; ++i) {
                ratchetModel_.materials[i].shader = worldHighpShader_;
            }
        }
        TraceLog(LOG_INFO, "ANDROID ES2: Loaded 32-bit highp vertex shader (ID %u)", worldHighpShader_.id);
    }
#endif
}

void RailGame::Shutdown() {
    SaveProgress();
    audio_.Shutdown();
    if (rivetHeadDiff_.id != 0)  UnloadTexture(rivetHeadDiff_);
    if (rivetHead2Diff_.id != 0) UnloadTexture(rivetHead2Diff_);
    if (rivetFur_.id != 0)       UnloadTexture(rivetFur_);
    if (rivetFur2_.id != 0)      UnloadTexture(rivetFur2_);
    if (rivetEyeDiff_.id != 0)     UnloadTexture(rivetEyeDiff_);
    if (ratchetHeadDiff_.id != 0)  UnloadTexture(ratchetHeadDiff_);
    if (ratchetHead2Diff_.id != 0) UnloadTexture(ratchetHead2Diff_);
    if (ratchetFur_.id != 0)       UnloadTexture(ratchetFur_);
    if (ratchetFur2_.id != 0)      UnloadTexture(ratchetFur2_);
    if (ratchetEyeDiff_.id != 0)   UnloadTexture(ratchetEyeDiff_);
    if (ratchetModelLoaded_) {
        for (int m = 0; m < ratchetModel_.materialCount; ++m) {
            ratchetModel_.materials[m].maps[MATERIAL_MAP_DIFFUSE].texture = Texture2D{};
        }
        UnloadModel(ratchetModel_);
        ratchetModelLoaded_ = false;
    }
    if (worldHighpShaderLoaded_) {
        UnloadShader(worldHighpShader_);
        worldHighpShaderLoaded_ = false;
    }
    if (splashLogo_.id != 0) {
        UnloadTexture(splashLogo_);
        splashLogo_.id = 0;
    }
    ShutdownUiFonts();
    if (renderTarget_.id != 0) {
        UnloadRenderTexture(renderTarget_);
    }
}

void RailGame::LoadSave() {
    std::ifstream in(SAVE_FILE_NAME, std::ios::binary);
    if (in.is_open()) {
        SaveData loaded{};
        in.read(reinterpret_cast<char*>(&loaded), sizeof(SaveData));
        std::streamsize bytesRead = in.gcount();
        if (bytesRead >= static_cast<std::streamsize>(sizeof(uint32_t)) &&
            (loaded.magic == 0x5241494E || loaded.magic == 0x5241494F)) {
            save_ = loaded;
            save_.magic = 0x5241494F;
            if (loaded.magic == 0x5241494E) {
                // Migrating from v2: Rebel Lombax (index 2) requires purchase (2500 bolts)
                save_.unlockedArmors[2] = false;
                if (save_.equippedArmor == 2) save_.equippedArmor = 0;
            }
            save_.unlockedWeapons[0] = true; // OmniWrench is always unlocked!
            save_.equippedWeapon = std::clamp(save_.equippedWeapon, 0, static_cast<int>(NUM_WEAPONS) - 1);
            if (!save_.unlockedWeapons[save_.equippedWeapon]) {
                save_.equippedWeapon = 0;
            }
            save_.equippedArmor = std::clamp(save_.equippedArmor, 0, static_cast<int>(NUM_ARMOR_SUITS) - 1);
            save_.equippedClankSkin = std::clamp(save_.equippedClankSkin, 0, static_cast<int>(NUM_CLANK_SKINS) - 1);
            save_.unlockedArmors[0] = true;
            save_.unlockedClankSkins[0] = true;
            save_.totalBoltsEarnedLifetime = std::max(save_.totalBoltsEarnedLifetime, save_.bolts);
            save_.totalRaritaniumEarnedLifetime = std::max(save_.totalRaritaniumEarnedLifetime, save_.raritanium);
            RefreshDailyContracts();
            audio_.SetMasterVolume(save_.masterVolume);
            audio_.SetMusicVolume(save_.musicVolume);
            audio_.SetSfxVolume(save_.sfxVolume);
            audio_.SetMusicTrackPreference(save_.musicTrackChoice);
            SetTargetFPS(save_.targetFps);
            return;
        }
    }
    save_ = SaveData{};
    RefreshDailyContracts();
}

void RailGame::SaveProgress() const {
    std::ofstream out(SAVE_FILE_NAME, std::ios::binary | std::ios::trunc);
    if (out.is_open()) {
        out.write(reinterpret_cast<const char*>(&save_), sizeof(SaveData));
    }
}

void RailGame::AwardSkillPoint(int idx) {
    if (idx < 0 || idx >= static_cast<int>(NUM_SKILL_POINTS)) return;
    if (save_.skillPoints[idx]) return;

    save_.skillPoints[idx] = true;
    const auto& sp = SKILL_POINT_CATALOG[idx];
    save_.bolts += sp.boltReward;
    save_.totalBoltsEarnedLifetime += sp.boltReward;
    SaveProgress();

    toast_.active = true;
    toast_.title = std::string("SKILL POINT UNLOCKED: ") + sp.title;
    toast_.subtitle = std::string(sp.requirement) + " (+" + std::to_string(sp.boltReward) + " Bolts)";
    toast_.timer = 4.2f;
    audio_.PlayLevelUp();
}

void RailGame::AddWeaponXp(int wIdx, int amount) {
    if (wIdx < 0 || wIdx >= static_cast<int>(NUM_WEAPONS)) return;
    if (save_.weaponLevels[wIdx] >= 5) return;

    save_.weaponXp[wIdx] += amount;
    int xpNeeded = save_.weaponLevels[wIdx] * 120;
    if (save_.weaponXp[wIdx] >= xpNeeded) {
        save_.weaponXp[wIdx] -= xpNeeded;
        save_.weaponLevels[wIdx]++;
        audio_.PlayLevelUp();

        toast_.active = true;
        toast_.title = std::string("WEAPON UPGRADED TO V") + std::to_string(save_.weaponLevels[wIdx]) + "!";
        toast_.subtitle = (save_.weaponLevels[wIdx] == 5)
                          ? WEAPON_CATALOG[wIdx].omegaName
                          : WEAPON_CATALOG[wIdx].name;
        toast_.timer = 3.5f;

        if (save_.weaponLevels[wIdx] >= 5) {
            AwardSkillPoint(5); // OMEGA ARSENAL
        }
        SaveProgress();
    }
}

void RailGame::RefreshDailyContracts() {
    std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);
    int dateKey = (lt->tm_year + 1900) * 10000 + (lt->tm_mon + 1) * 100 + lt->tm_mday;
    bool validIds = true;
    for (size_t i = 0; i < NUM_DAILY_CONTRACTS; ++i) {
        if (save_.dailyContractIds[i] < 0 || save_.dailyContractIds[i] >= static_cast<int>(CONTRACT_POOL_SIZE)) {
            validIds = false;
        }
    }
    if (save_.dailyContractDateKey == dateKey && validIds) return;

    save_.dailyContractDateKey = dateKey;
    PCG32 cRng(static_cast<uint64_t>(dateKey) * 2654435761ULL, 77ULL);
    std::array<int, CONTRACT_POOL_SIZE> order{};
    for (size_t i = 0; i < CONTRACT_POOL_SIZE; ++i) order[i] = static_cast<int>(i);
    for (int i = static_cast<int>(CONTRACT_POOL_SIZE) - 1; i > 0; --i) {
        int j = cRng.RangeInt(0, i);
        std::swap(order[i], order[j]);
    }
    for (size_t i = 0; i < NUM_DAILY_CONTRACTS; ++i) {
        save_.dailyContractIds[i] = order[i];
        save_.dailyContractProg[i] = 0;
        save_.dailyContractDone[i] = false;
    }
    SaveProgress();
}

void RailGame::ProgressContract(int contractType, int amount) {
    if (amount <= 0) return;
    RefreshDailyContracts();
    for (size_t i = 0; i < NUM_DAILY_CONTRACTS; ++i) {
        if (save_.dailyContractDone[i]) continue;
        if (save_.dailyContractIds[i] != contractType) continue;
        const auto& def = GetContractDef(contractType);
        int gain = (isDailyRun_ && def.targetCount > 1) ? (amount * 2) : amount;
        save_.dailyContractProg[i] = std::min(def.targetCount, save_.dailyContractProg[i] + gain);
        if (save_.dailyContractProg[i] >= def.targetCount) {
            save_.dailyContractDone[i] = true;
            save_.totalContractsCompleted++;
            save_.bolts += def.boltReward;
            save_.totalBoltsEarnedLifetime += def.boltReward;
            save_.raritanium += def.rariReward;
            save_.totalRaritaniumEarnedLifetime += def.rariReward;
            runBoltsEarned_ += def.boltReward;
            runRaritaniumEarned_ += def.rariReward;
            SaveProgress();

            toast_.active = true;
            toast_.title = std::string("CONTRACT COMPLETE: ") + def.title;
            toast_.subtitle = "+" + std::to_string(def.boltReward) + " Bolts & +" + std::to_string(def.rariReward) + " Raritanium!";
            toast_.timer = 4.0f;
            audio_.PlayLevelUp();
        }
    }
}

void RailGame::TriggerLiveRadioDare() {
    if (activeDare_.active || boss_.active) return;
    int pick = rng_.RangeInt(0, 6);
    if (pick == lastDareType_) pick = (pick + 1 + rng_.RangeInt(0, 5)) % 7;
    lastDareType_ = pick;

    activeDare_ = LiveRadioDare{};
    activeDare_.active = true;
    activeDare_.dareType = pick;
    activeDare_.progress = 0;
    activeDare_.startDistance = distanceMeters_;

    switch (pick) {
        case 0:
            activeDare_.speaker = CommSpeaker::CAPTAIN_QWARK;
            activeDare_.speakerName = "CAPTAIN QWARK";
            activeDare_.title = "WRENCH FU DARE";
            activeDare_.prompt = "Holster that blaster, cadet! Smash 5 crates or foes with your OmniWrench!";
            activeDare_.target = 5;
            activeDare_.maxTimer = 18.0f;
            break;
        case 1:
            activeDare_.speaker = CommSpeaker::DR_NEFARIOUS;
            activeDare_.speakerName = "DR. NEFARIOUS";
            activeDare_.title = "UNTOUCHABLE TAUNT";
            activeDare_.prompt = "Bwahaha! Bet you can't grind 180m without taking a scratch OR crate bump!";
            activeDare_.target = 180;
            activeDare_.maxTimer = 20.0f;
            break;
        case 2:
            activeDare_.speaker = CommSpeaker::CAPTAIN_QWARK;
            activeDare_.speakerName = "CAPTAIN QWARK";
            activeDare_.title = "DRONE SAFARI";
            activeDare_.prompt = "Hostiles ahead! Blast or smash 3 Drones or Enforcers before time runs out!";
            activeDare_.target = 3;
            activeDare_.maxTimer = 20.0f;
            break;
        case 3:
            activeDare_.speaker = CommSpeaker::CLANK;
            activeDare_.speakerName = "CLANK";
            activeDare_.title = "AERIAL CALIBRATION";
            activeDare_.prompt = "Scanning high trajectories! Collect 10 airborne Bolts or grapple a Swingshot!";
            activeDare_.target = 10;
            activeDare_.maxTimer = 18.0f;
            break;
        case 4:
            activeDare_.speaker = CommSpeaker::DR_NEFARIOUS;
            activeDare_.speakerName = "DR. NEFARIOUS";
            activeDare_.title = "PYROTECHNICS TEST";
            activeDare_.prompt = "Give me explosions! Detonate a TNT Barrel or smash 6 objects on the rails!";
            activeDare_.target = 6;
            activeDare_.maxTimer = 18.0f;
            break;
        case 5:
            activeDare_.speaker = CommSpeaker::CLANK;
            activeDare_.speakerName = "CLANK";
            activeDare_.title = "RAIL WEAVER";
            activeDare_.prompt = "Testing Magne-Boot agility! Execute 6 clean rail hops without stumbling!";
            activeDare_.target = 6;
            activeDare_.maxTimer = 16.0f;
            break;
        default:
            activeDare_.speaker = CommSpeaker::CAPTAIN_QWARK;
            activeDare_.speakerName = "CAPTAIN QWARK";
            activeDare_.title = "COMBO SHOWTIME";
            activeDare_.prompt = "Give the holo-cameras a show! Score 7 Multiplier hits before my coffee cools!";
            activeDare_.target = 7;
            activeDare_.maxTimer = 18.0f;
            break;
    }
    activeDare_.timer = activeDare_.maxTimer;
    radioComm_.active = false; // Hand holographic comm channel to the live dare
    audio_.PlayRadioChirp();
}

void RailGame::AdvanceRadioDare(int dareEventType, int amount) {
    if (!activeDare_.active || activeDare_.dareType != dareEventType) return;
    activeDare_.progress = std::min(activeDare_.target, activeDare_.progress + amount);
    if (activeDare_.progress >= activeDare_.target) {
        CommSpeaker spk = activeDare_.speaker;
        std::string spkName = activeDare_.speakerName;
        activeDare_.active = false;
        save_.totalRadioDaresCompleted++;
        int rariBonus = isDailyRun_ ? 2 : 1;
        SpawnBoltShower({ player_.x, player_.y + 1.2f, 2.5f }, 7 + rariBonus, rariBonus);
        audio_.PlayRaritaniumPickup();
        SaveProgress();

        if (spk == CommSpeaker::DR_NEFARIOUS) {
            TriggerRadioTransmission(spk, spkName, "WHAT?! IMPOSSIBLE! LAWRENCE, WHO CALIBRATED THAT OBSTACLE COURSE?!");
        } else if (spk == CommSpeaker::CLANK) {
            TriggerRadioTransmission(spk, spkName, "Telemetry confirmed, Ratchet! Supply cache & Raritanium deployed ahead.");
        } else {
            TriggerRadioTransmission(spk, spkName, "Magnificent! Almost as graceful as me! Catch your bonus Raritanium, cadet!");
        }
        radioComm_.timer = 3.0f;
        radioComm_.maxTimer = 3.0f;
    }
}

void RailGame::FailRadioDare(const char* failReason) {
    if (!activeDare_.active) return;
    CommSpeaker spk = activeDare_.speaker;
    std::string spkName = activeDare_.speakerName;
    activeDare_.active = false;
    TriggerRadioTransmission(spk, spkName, failReason);
    radioComm_.timer = 2.4f;
    radioComm_.maxTimer = 2.4f;
}

bool RailGame::HasAffordableArsenalTab(int tab) const {
    if (tab == 0) {
        const auto& cat = GetWeaponCatalog();
        for (size_t i = 0; i < NUM_WEAPONS; ++i) {
            if (!save_.unlockedWeapons[i]) {
                if (save_.bolts >= cat[i].unlockBoltCost) return true;
            } else {
                int lvl = save_.weaponLevels[i];
                if (lvl < 5) {
                    if (save_.raritanium >= cat[i].upgradeRaritaniumCost * lvl) return true;
                } else if (!save_.goldenWeapons[i]) {
                    if (save_.raritanium >= GOLDEN_WEAPON_CATALOG[i].unlockRaritaniumCost &&
                        save_.bolts >= GOLDEN_WEAPON_CATALOG[i].unlockBoltCost) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    if (tab == 1) {
        if (save_.armorLevel < 4 && save_.bolts >= 450 * (save_.armorLevel + 1)) return true;
        if (save_.magnetLevel < 5 && save_.bolts >= 300 * (save_.magnetLevel + 1)) return true;
        if (save_.bootThrusterLevel < 5 && save_.bolts >= 350 * (save_.bootThrusterLevel + 1)) return true;
        if (save_.multiplierModLevel < 5 && save_.bolts >= 400 * (save_.multiplierModLevel + 1)) return true;
        if (!save_.hoverbootsUnlocked && save_.bolts >= 1800 && save_.raritanium >= 10) return true;
        return false;
    }
    if (tab == 2) {
        for (size_t i = 0; i < NUM_ARMOR_SUITS; ++i) {
            if (!save_.unlockedArmors[i] &&
                save_.bolts >= ARMOR_CATALOG[i].boltCost &&
                save_.raritanium >= ARMOR_CATALOG[i].raritaniumCost) {
                return true;
            }
        }
        for (size_t i = 0; i < NUM_CLANK_SKINS; ++i) {
            if (!save_.unlockedClankSkins[i] &&
                save_.bolts >= CLANK_SKIN_CATALOG[i].boltCost &&
                save_.raritanium >= CLANK_SKIN_CATALOG[i].raritaniumCost) {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool RailGame::HasAnyAffordableUpgrade() const {
    for (int t = 0; t < 3; ++t) {
        if (HasAffordableArsenalTab(t)) return true;
    }
    return false;
}

void RailGame::StartNewRun(bool dailySeed) {
    RefreshDailyContracts();
    isDailyRun_ = dailySeed;
    activeMutator_ = GetTodaysMutator();
    activeDare_ = LiveRadioDare{};
    nextDareDistance_ = 220.0f;
    runDistanceCarry_ = 0.0f;
    unpauseRampTimer_ = 0.0f;
    if (dailySeed) {
        activeSeed_ = PCG32::GetDailyChallengeSeed();
    } else {
        activeSeed_ = static_cast<uint64_t>(std::time(nullptr)) ^
                      static_cast<uint64_t>(GetTime() * 100000.0);
    }
    rng_.Seed(activeSeed_, isDailyRun_ ? 999ULL : 42ULL);

    SetScreenMode(ScreenMode::PLAYING);
    save_.totalRunsStarted++;
    runTime_ = 0.0f;
    distanceMeters_ = 0.0f;
    totalWorldTravel_ = 0.0f;
    grindSpeed_ = isDailyRun_ ? 42.0f : 38.0f;
    nextChunkZ_ = 36.0f;
    distanceSinceBoss_ = 0.0f;

    runBoltsEarned_ = 0;
    runRaritaniumEarned_ = 0;
    runKills_ = 0;
    runCrates_ = 0;
    for (int i = 0; i < NUM_SECTORS; ++i) {
        sectorOrder_[i] = static_cast<GalaxySector>(i);
    }
    for (int i = NUM_SECTORS - 1; i > 0; --i) {
        int j = rng_.RangeInt(0, i);
        std::swap(sectorOrder_[i], sectorOrder_[j]);
    }
    lastSector_ = sectorOrder_[0];
    sectorBannerTimer_ = 3.2f;

    // Reset Lombax Player
    player_ = PlayerLombax{};
    player_.currentLane = 2; // Center lane on 5-lane grid (0=FarL, 1=MidL, 2=Center, 3=MidR, 4=FarR)
    player_.x = GetLaneX(2);
    player_.y = 0.0f;
    player_.isHopping = false;
    player_.maxNanotechHp = 4.0f + static_cast<float>(save_.armorLevel);
    if (save_.equippedArmor == static_cast<int>(ArmorSuitId::CARBONOX)) {
        player_.maxNanotechHp += 1.0f; // Carbonox Armor Perk: +1 Extra Nanotech HP pip!
    }
    player_.nanotechHp = player_.maxNanotechHp;
    player_.maxHeliFuel = 1.2f + save_.bootThrusterLevel * 0.25f;
    if (IsMutatorActive(DailyMutatorId::AERIAL_ASSAULT)) {
        player_.maxHeliFuel *= 2.0f;
    }
    player_.heliFuel = player_.maxHeliFuel;
    player_.multiplierTier = 1;
    player_.multiplierProgress = 0.0f;
    static constexpr std::array<int, NUM_WEAPONS> BASE_AMMO = { 999, 45, 24, 30, 18, 20 };
    for (int w = 0; w < static_cast<int>(NUM_WEAPONS); ++w) {
        if (w == 0) {
            player_.weaponMaxAmmo[w] = 999;
        } else {
            int lvlBonus = (save_.weaponLevels[w] - 1) * std::max(2, BASE_AMMO[w] / 8);
            int maxCap = BASE_AMMO[w] + lvlBonus;
            if (IsMutatorActive(DailyMutatorId::GADGETRON_OVERLOAD)) {
                maxCap = (maxCap * 3) / 2;
            }
            player_.weaponMaxAmmo[w] = maxCap;
        }
        player_.weaponAmmo[w] = player_.weaponMaxAmmo[w];
    }

    if (isDailyRun_) {
        const auto& mDef = MUTATOR_CATALOG[static_cast<int>(activeMutator_)];
        TriggerRadioTransmission(CommSpeaker::CAPTAIN_QWARK,
                                 std::string("DAILY MUTATOR // ") + mDef.name,
                                 mDef.perkDesc);
    }

    boss_ = BossDropship{};
    for (auto& h : hazards_) h.active = false;
    for (auto& b : bolts_) b.active = false;
    for (auto& p : projectiles_) p.active = false;
    for (auto& pt : particles_) pt.active = false;

    // Spawn starter line of shiny Bolts right in front of Ratchet on the center rail!
    SpawnRailBoltLine(2, 10.0f, 7, 2.6f, false);

    for (int i = 0; i < 4; ++i) {
        SpawnChunkPattern(nextChunkZ_);
    }

    audio_.PlayRailHop();
}

float RailGame::GetTrackCurveX(float z) const {
    if (z <= 0.0f) return 0.0f;
    float zn = z / 140.0f;
    float phase = distanceMeters_ * 0.032f + globalTime_ * 0.30f;
    return (std::sin(phase + zn * 2.5f) - std::sin(phase)) * (zn * zn) * 6.5f;
}

float RailGame::GetTrackCurveY(float z) const {
    if (z <= 0.0f) return 0.0f;
    float zn = z / 140.0f;
    float phase = distanceMeters_ * 0.026f + globalTime_ * 0.22f;
    // Rails dip slightly in mid-range and sweep upward into the distant reactor portal at z = 148!
    float wave = (std::cos(phase + zn * 2.2f) - std::cos(phase)) * (zn * zn) * 2.0f;
    float rise = (zn * zn * zn) * 5.8f;
    return wave + rise;
}

Vector3 RailGame::ApplyTrackCurve(float laneX, float y, float z) const {
    // As rails approach the distant circular reactor portal (z = 85..148), they converge into the reactor ports!
    float tPinch = std::clamp((z - 80.0f) / 68.0f, 0.0f, 1.0f);
    float pinch = 1.0f - (tPinch * tPinch) * 0.65f;
    return {
        laneX * pinch + GetTrackCurveX(z),
        y + GetTrackCurveY(z),
        z
    };
}

bool RailGame::IsRailGapAt(int lane, float z) const {
    if (mode_ != ScreenMode::PLAYING && mode_ != ScreenMode::PAUSED && mode_ != ScreenMode::GAME_OVER) {
        return false;
    }
    for (const auto& h : hazards_) {
        if (!h.active || h.kind != HazardKind::BROKEN_RAIL_GAP || h.lane != lane) continue;
        float half = h.lengthZ * 0.5f;
        if (z >= (h.z - half) && z <= (h.z + half)) {
            return true;
        }
    }
    return false;
}

int RailGame::GetCorridorMinLaneAt(float worldZ) const {
    if (mode_ != ScreenMode::PLAYING && mode_ != ScreenMode::PAUSED && mode_ != ScreenMode::GAME_OVER) {
        return 1; // In menus, always show the central 3 rails (Lanes 1, 2, 3)
    }
    // 240-meter continuous 3D world blocks:
    // Block 0: Central Corridor (Lanes 1, 2, 3)
    // Block 1: Left Sky-Freeway (Lanes 0, 1, 2)
    // Block 2: Re-converging Central Hub (Lanes 1, 2, 3)
    // Block 3: Right Megacorp Overpass (Lanes 2, 3, 4)
    int block = static_cast<int>(std::floor(std::max(0.0f, worldZ) / 240.0f));
    int pat = block % 4;
    if (pat == 1) return 0; // Left 3 rails
    if (pat == 3) return 2; // Right 3 rails
    return 1;               // Center 3 rails
}

bool RailGame::IsRailActiveAt(int lane, float z) const {
    if (lane < 0 || lane >= NUM_LANES) return false;
    float worldZ = totalWorldTravel_ + z;
    int minLane = GetCorridorMinLaneAt(worldZ);
    if (lane < minLane || lane > minLane + 2) {
        return false; // Strictly capped at 3 active rails at a time!
    }
    return !IsRailGapAt(lane, z);
}

GalaxySector RailGame::GetSectorAtDistance(float distMeters) const {
    int stage = (static_cast<int>(std::max(0.0f, distMeters)) / 500) % NUM_SECTORS;
    if (static_cast<int>(sectorOrder_[0]) == 0 && static_cast<int>(sectorOrder_[1]) == 0) {
        return static_cast<GalaxySector>(stage);
    }
    return sectorOrder_[stage];
}

GalaxySector RailGame::GetCurrentSector() const {
    return GetSectorAtDistance(distanceMeters_);
}

const char* RailGame::GetSectorName(GalaxySector s) const {
    switch (s) {
        case GalaxySector::METROPOLIS_KERWAN: return "SECTOR 01 // METROPOLIS KERWAN (SOLANA GALAXY)";
        case GalaxySector::CORSON_V:          return "SECTOR 02 // CORSON V: NEFARIOUS CITY (POLARIS)";
        case GalaxySector::GASPAR_VOLCANIC:   return "SECTOR 03 // PLANET GASPAR: VOLCANIC CORE";
        case GalaxySector::POKITARU_RESORT:   return "SECTOR 04 // POKITARU: JOWAI RESORT COAST";
        case GalaxySector::TORREN_IV_SCRAP:   return "SECTOR 05 // TORREN IV: MOLONOTH SCRAP FIELDS";
        case GalaxySector::MEGACORP_FACILITY: return "SECTOR 06 // MEGACORP WEAPONS PARK (BOGON GALAXY)";
        case GalaxySector::ZORDOOM_PRISON:    return "SECTOR 07 // ZORDOOM PRISON MAXIMUM SECURITY";
        case GalaxySector::DIMENSIONATOR_RIFT:return "SECTOR 08 // DIMENSIONATOR RIFT ANOMALY";
    }
    return "UNKNOWN SECTOR";
}

Vector2 RailGame::GetVirtualMouse() const {
    return ToVirtualCoords(GetMousePosition());
}

Vector2 RailGame::ToVirtualCoords(Vector2 screenPos) const {
    float sw = static_cast<float>(GetScreenWidth());
    float sh = static_cast<float>(GetScreenHeight());
    float rw = static_cast<float>(GetRenderWidth());
    float rh = static_cast<float>(GetRenderHeight());
    if (sw <= 0.0f || sh <= 0.0f || uiScale_ <= 0.0f) return { 0.0f, 0.0f };
    float rx = (rw > 0.0f) ? (screenPos.x * (rw / sw)) : screenPos.x;
    float ry = (rh > 0.0f) ? (screenPos.y * (rh / sh)) : screenPos.y;
    return {
        (rx - uiOffsetX_) / uiScale_,
        (ry - uiOffsetY_) / uiScale_
    };
}

bool RailGame::IsRectHovered(Rectangle r) const {
    if (CheckCollisionPointRec(GetVirtualMouse(), r)) return true;
    int tc = GetTouchPointCount();
    for (int i = 0; i < tc; ++i) {
        if (CheckCollisionPointRec(ToVirtualCoords(GetTouchPosition(i)), r)) return true;
    }
    return false;
}

void RailGame::SetScreenMode(ScreenMode nextMode) {
    if (mode_ == nextMode) return;
    mode_ = nextMode;
    screenTransitionCooldown_ = 0.25f; // Prevent input bleed across screen transitions
}

bool RailGame::IsButtonClicked(Rectangle r) const {
    if (screenTransitionCooldown_ > 0.0f) return false;
    Rectangle padded = { r.x - 10.0f, r.y - 10.0f, r.width + 20.0f, r.height + 20.0f };
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetVirtualMouse(), padded)) {
        return true;
    }
    int tc = GetTouchPointCount();
    for (int i = 0; i < tc; ++i) {
        int tid = GetTouchPointId(i);
        bool isNew = true;
        for (int p = 0; p < prevTouchCount_; ++p) {
            if (prevTouchIds_[p] == tid) { isNew = false; break; }
        }
        if (isNew && CheckCollisionPointRec(ToVirtualCoords(GetTouchPosition(i)), padded)) {
            return true;
        }
    }
    return false;
}

void RailGame::SwitchLane(int delta) {
    if (player_.isHopping || player_.laneSwitchCooldown > 0.0f) {
        // Buffer intentional lane change rather than allowing mid-air hop-canceling
        player_.bufferedLaneDelta = (delta > 0) ? 1 : -1;
        player_.bufferedLaneTimer = 0.14f;
        return;
    }
    int rawTarget = player_.currentLane + delta;
    if (rawTarget < 0 || rawTarget >= NUM_LANES) return;
    int minL = GetCorridorMinLaneAt(totalWorldTravel_);
    int maxL = minL + 2;
    if (rawTarget < minL || rawTarget > maxL) {
        return; // Strictly prevent hopping onto non-existent rails outside active 3-rail corridor!
    }
    int target = rawTarget;
    if (target != player_.currentLane) {
        player_.currentLane = target;
        player_.isHopping = true;
        player_.hopTimer = 0.0f;
        player_.hopDuration = 0.29f; // Deliberate lateral leap with full commitment (~0.29s)
        player_.hopStartX = player_.x;
        player_.hopTargetX = GetLaneX(target);
        player_.hopDir = (delta > 0) ? 1 : -1;
        player_.landingSquashTimer = 0.0f;
        player_.bufferedLaneDelta = 0;
        player_.bufferedLaneTimer = 0.0f;

        // Radial burst of magnetic grind boot disengagement sparks
        SpawnParticleBurst3D({ player_.x, player_.y + 0.1f, 0.0f }, 12, SKYBLUE, 3.0f, 8.0f, 0.20f, 0.28f);
        audio_.PlayRailHop();
        AdvanceRadioDare(5, 1);
    }
}

void RailGame::TriggerJumpOrGlide() {
    player_.slideTimer = 0.0f;
    if (player_.grounded) {
        player_.grounded = false;
        player_.usedAirBoost = false;
        player_.vy = 18.5f;
        player_.heliGliding = false;
        audio_.PlayJump();
        SpawnParticleBurst3D({ player_.x, 0.1f, 0.0f }, 15, SKYBLUE, 2.0f, 7.0f, 0.24f, 0.35f);
    } else if (player_.heliFuel > 0.0f) {
        // Engage Clank Heli-Pack Thruster Glide; upward double-jump lift triggers only once per leap!
        player_.heliGliding = true;
        if (!player_.usedAirBoost) {
            player_.usedAirBoost = true;
            if (player_.vy < 5.0f && player_.y < 4.15f) {
                player_.vy = 8.5f;
            }
            audio_.PlayJump();
            SpawnParticleBurst3D({ player_.x, player_.y + 0.5f, -0.25f }, 10, SKYBLUE, 2.0f, 6.0f, 0.16f, 0.25f);
        }
    }
}

void RailGame::TriggerWrenchSlide() {
    player_.slideTimer = 0.65f;
    player_.heliGliding = false;
    if (!player_.grounded) {
        player_.vy = -26.0f; // Fast drop back onto the rail!
    }
    audio_.PlayRailHop();
}

void RailGame::CycleEquippedWeapon(int delta) {
    int start = save_.equippedWeapon;
    for (int i = 1; i <= static_cast<int>(NUM_WEAPONS); ++i) {
        int cand = (start + delta * i + static_cast<int>(NUM_WEAPONS) * 4) % static_cast<int>(NUM_WEAPONS);
        if (save_.unlockedWeapons[cand]) {
            save_.equippedWeapon = cand;
            audio_.PlayUiClick();
            break;
        }
    }
}

void RailGame::GrantWeaponAmmo(int percent, bool allGuns) {
    if (allGuns) {
        for (int w = 1; w < static_cast<int>(NUM_WEAPONS); ++w) {
            if (!save_.unlockedWeapons[w]) continue;
            int add = std::max(1, (player_.weaponMaxAmmo[w] * percent) / 100);
            player_.weaponAmmo[w] = std::min(player_.weaponMaxAmmo[w], player_.weaponAmmo[w] + add);
        }
        return;
    }
    int target = -1;
    int eq = save_.equippedWeapon;
    if (eq > 0 && save_.unlockedWeapons[eq] && player_.weaponAmmo[eq] < player_.weaponMaxAmmo[eq]) {
        target = eq;
    } else {
        float lowestFrac = 1.0f;
        for (int w = 1; w < static_cast<int>(NUM_WEAPONS); ++w) {
            if (!save_.unlockedWeapons[w]) continue;
            float frac = static_cast<float>(player_.weaponAmmo[w]) / static_cast<float>(std::max(1, player_.weaponMaxAmmo[w]));
            if (frac < lowestFrac) {
                lowestFrac = frac;
                target = w;
            }
        }
    }
    if (target > 0) {
        int add = std::max(1, (player_.weaponMaxAmmo[target] * percent) / 100);
        player_.weaponAmmo[target] = std::min(player_.weaponMaxAmmo[target], player_.weaponAmmo[target] + add);
    }
}

void RailGame::TriggerFireWeapon() {
    if (player_.weaponCooldown > 0.0f) return;

    int wIdx = save_.equippedWeapon;
    if (wIdx > 0 && player_.weaponAmmo[wIdx] <= 0) {
        // Out of ammo: auto-fallback to unlimited OmniWrench 8000 so player can smash crates!
        save_.equippedWeapon = 0;
        wIdx = 0;
    }
    if (wIdx > 0) {
        player_.weaponAmmo[wIdx] = std::max(0, player_.weaponAmmo[wIdx] - 1);
    }
    const auto& wDef = WEAPON_CATALOG[wIdx];
    int wLvl = save_.weaponLevels[wIdx];
    bool isGolden = save_.goldenWeapons[wIdx];
    float dmg = wDef.baseDamage * (1.0f + (wLvl - 1) * 0.28f);
    if (isGolden) dmg *= 1.45f;
    if (IsMutatorActive(DailyMutatorId::ARMORED_CONVOY) && (wIdx == 0 || wIdx == 1)) {
        dmg *= 2.0f;
    }

    player_.weaponCooldown = wDef.fireCooldown * std::max(0.65f, 1.0f - (wLvl - 1) * 0.06f);
    if (isGolden) player_.weaponCooldown *= 0.85f; // Faster golden cycle
    if (IsMutatorActive(DailyMutatorId::GADGETRON_OVERLOAD)) {
        player_.weaponCooldown *= 0.70f;
    }
    WeaponId wId = static_cast<WeaponId>(wIdx);
    Color projColor = isGolden ? GOLDEN_WEAPON_CATALOG[wIdx].projectileColor : wDef.projectileColor;
    Vector3 muzzle = { player_.x - 0.38f, player_.y + 1.18f, 1.35f };
    if (wId != WeaponId::OMNIWRENCH_8000) {
        player_.gunAimTimer = 0.85f;
        player_.gunRecoilTimer = 0.20f;
        SpawnParticleBurst3D(muzzle, isGolden ? 12 : 6, projColor, 3.0f, 9.0f, 0.18f, 0.16f);
    }

    if (wId == WeaponId::OMNIWRENCH_8000) {
        // SPECIAL 1: Forward Melee Wrench Strike (smashes close targets + deflects enemy shots back!)
        player_.wrenchSwingTimer = 0.36f;
        audio_.PlayWrenchSmash();
        if (isGolden) {
            SpawnParticleBurst3D({ player_.x, player_.y + 0.8f, 1.8f }, 18, GOLD, 3.5f, 11.0f, 0.26f, 0.30f);
        }

        // Deflect any incoming enemy plasma shots in front of Ratchet right back down the rail!
        for (auto& ep : projectiles_) {
            if (ep.active && !ep.friendly && ep.pos.z > -1.0f && ep.pos.z < 9.5f &&
                std::abs(ep.pos.x - player_.x) < 2.2f) {
                ep.friendly = true;
                ep.sourceWeapon = WeaponId::OMNIWRENCH_8000;
                ep.vel = { 0.0f, 0.0f, 95.0f };
                ep.damage = dmg;
                ep.color = isGolden ? GOLD : SKYBLUE;
                ep.life = 2.0f;
                SpawnParticleBurst3D(ep.pos, 12, ep.color, 3.0f, 10.0f, 0.22f, 0.30f);
                ProgressContract(9, 1);
            }
        }
    } else if (wId == WeaponId::COMBUSTER) {
        // SPECIAL 2: Rapid-fire Magma Stream (2.5x bonus shield-melting damage vs Enforcers + multi-stream!)
        audio_.PlayBlasterShot();
        SpawnProjectile(muzzle, { 0.0f, 0.0f, 98.0f }, player_.currentLane, wId, dmg, 0.55f, projColor, true, 0);
        if (wLvl >= 3 || isGolden) {
            SpawnProjectile({ muzzle.x + 0.32f, muzzle.y + 0.06f, muzzle.z - 0.4f }, { 0.0f, 0.0f, 102.0f },
                            player_.currentLane, wId, dmg * 0.70f, 0.48f, isGolden ? GOLD : YELLOW, true, 0);
        }
        if (wLvl >= 5 || isGolden) {
            SpawnProjectile({ muzzle.x - 0.32f, muzzle.y - 0.04f, muzzle.z - 0.8f }, { 0.0f, 0.0f, 105.0f },
                            player_.currentLane, wId, dmg * 0.70f, 0.48f, isGolden ? GOLD : ORANGE, true, 0);
        }
    } else if (wId == WeaponId::SHOCK_BLASTER) {
        // SPECIAL 3: 3-Rail Shockwave Spread + Vaporizes Enemy Plasma Shots in Mid-Air!
        audio_.PlayShotgunBlast();
        cameraShake_ = std::max(cameraShake_, 0.25f);
        for (auto& ep : projectiles_) {
            if (ep.active && !ep.friendly && ep.pos.z > 0.0f && ep.pos.z < 42.0f) {
                ep.active = false;
                SpawnParticleBurst3D(ep.pos, 10, isGolden ? GOLD : SKYBLUE, 2.5f, 9.0f, 0.20f, 0.25f);
                ProgressContract(9, 1);
            }
        }
        for (int lane = 0; lane < NUM_LANES; ++lane) {
            float targetX = GetLaneX(lane);
            float vx = (targetX - muzzle.x) * 3.2f;
            SpawnProjectile(muzzle, { vx, 0.0f, 84.0f }, lane, wId, dmg, 0.88f, projColor, true, 0);
        }
    } else if (wId == WeaponId::BUZZ_BLADES) {
        // SPECIAL 4: Smart Cross-Rail Pinball Ricochet (bounces between targets across all 3 rails!)
        audio_.PlayBlasterShot();
        int bounces = (isGolden ? 10 : 3) + wLvl;
        SpawnProjectile(muzzle, { 0.0f, 0.0f, 92.0f }, player_.currentLane, wId, dmg, 0.68f, projColor, true, bounces);
        if (isGolden) {
            SpawnProjectile({ muzzle.x - 0.3f, muzzle.y, muzzle.z }, { -6.0f, 0.0f, 88.0f }, player_.currentLane, wId, dmg * 0.8f, 0.60f, GOLD, true, bounces);
            SpawnProjectile({ muzzle.x + 0.3f, muzzle.y, muzzle.z }, { 6.0f, 0.0f, 88.0f }, player_.currentLane, wId, dmg * 0.8f, 0.60f, GOLD, true, bounces);
        }
    } else if (wId == WeaponId::PLASMA_STRIKER) {
        // SPECIAL 5: Infinite Rail-Piercing Hyperbeam + EMP Chain-Lightning to Adjacent Rails!
        audio_.PlayRailgunBeam();
        cameraShake_ = std::max(cameraShake_, 0.35f);
        SpawnProjectile(muzzle, { 0.0f, 0.0f, 165.0f }, player_.currentLane, wId, dmg, 0.85f, projColor, true, 20);
    } else if (wId == WeaponId::RYNO_INFINITE) {
        // SPECIAL 6: All-Rail Homing Micro-Missile Swarm + Explosive AoE & +50% Bonus Bolt Jackpot!
        audio_.PlayShotgunBlast();
        cameraShake_ = std::max(cameraShake_, 0.45f);
        for (int lane = 0; lane < NUM_LANES; ++lane) {
            Vector3 mPos = { GetLaneX(lane), muzzle.y + 0.4f, muzzle.z };
            SpawnProjectile(mPos, { 0.0f, 0.0f, 108.0f }, lane, wId, dmg, 0.95f, projColor, true, 1);
        }
    }
}

void RailGame::AdvanceMultiplier() {
    AdvanceRadioDare(6, 1);
    float step = 0.22f + save_.multiplierModLevel * 0.04f;
    if (save_.equippedArmor == static_cast<int>(ArmorSuitId::LOMBAX_PRAETORIAN)) {
        step *= 1.50f; // Lombax Praetorian Perk: +50% faster Bolt Multiplier escalation!
    }
    if (IsMutatorActive(DailyMutatorId::OVERCLOCKED_RAILS)) {
        step *= 1.60f;
    }
    player_.multiplierProgress += step;
    if (player_.multiplierProgress >= 1.0f) {
        player_.multiplierProgress = 0.0f;
        if (player_.multiplierTier == 1) player_.multiplierTier = 2;
        else if (player_.multiplierTier == 2) player_.multiplierTier = 4;
        else if (player_.multiplierTier == 4) player_.multiplierTier = 8;
        else if (player_.multiplierTier == 8) {
            player_.multiplierTier = 12;
            ProgressContract(5, 1);
        }
        else if (player_.multiplierTier >= 12) {
            player_.multiplierTier = 20;
            ProgressContract(5, 1);
            AwardSkillPoint(2); // MAXIMUM MULTIPLIER
        }
    }
}

void RailGame::TriggerOverdriveBoost() {
    if (!save_.hoverbootsUnlocked) return;
    if (player_.overdriveCooldown > 0.0f) return;
    save_.totalOverdriveBoosts++;
    ProgressContract(10, 1);
    player_.overdriveDuration = 3.0f;
    player_.overdriveCooldown = 11.0f;
    player_.invulnTimer = 3.0f;
    player_.hyperBoostTimer = 3.0f;
    touchFeedbackBoost_ = 0.25f;
    cameraShake_ = 0.45f;
    screenFlashColor_ = GOLD;
    screenFlashAlpha_ = 0.35f;
    audio_.PlayHyperBoost();
    SpawnParticleBurst3D({ player_.x, player_.y + 0.2f, 0.0f }, 30, GOLD, 4.0f, 15.0f, 0.35f, 0.45f);
}

void RailGame::TriggerSwingshotGrapple(Vector3 nodePos, int targetLane) {
    if (player_.isSwinging) return;
    save_.totalSwingshotGrapples++;
    ProgressContract(3, 1);
    AdvanceRadioDare(3, 5);
    player_.isSwinging = true;
    player_.swingTimer = 0.0f;
    player_.swingStartX = player_.x;
    player_.swingTargetLane = targetLane;
    player_.swingNodePos = nodePos;
    audio_.PlaySwingshot();
    cameraShake_ = 0.25f;
    SpawnParticleBurst3D(nodePos, 20, GREEN, 2.5f, 9.0f, 0.25f, 0.4f);
}

void RailGame::TriggerRadioTransmission(CommSpeaker speaker, const std::string& name, const std::string& text) {
    radioComm_.active = true;
    radioComm_.speaker = speaker;
    radioComm_.speakerName = name;
    radioComm_.text = text;
    radioComm_.timer = 4.8f;
    radioComm_.maxTimer = 4.8f;
    audio_.PlayRadioChirp();
}

void RailGame::DamagePlayer(float dmg, bool minorBump) {
    if (player_.invulnTimer > 0.0f || player_.hyperBoostTimer > 0.0f) return;

    if (activeDare_.active && (activeDare_.dareType == 1 || activeDare_.dareType == 5)) {
        FailRadioDare(activeDare_.dareType == 1
                      ? "BWAHAHA! Too clumsy, Lombax! Untouchable wager VOIDED!"
                      : "Impact detected, Ratchet. Agility calibration aborted.");
    }

    player_.nanotechHp -= dmg;
    player_.invulnTimer = minorBump ? 0.95f : 1.65f;
    if (!minorBump) {
        player_.multiplierTier = 1;
        player_.multiplierProgress = 0.0f;
    } else {
        player_.multiplierProgress = 0.0f;
    }
    cameraShake_ = minorBump ? 0.42f : 0.75f;
    screenFlashColor_ = minorBump ? ORANGE : RED;
    screenFlashAlpha_ = minorBump ? 0.35f : 0.55f;
    audio_.PlayHurt();

    SpawnParticleBurst3D({ player_.x, player_.y + 1.0f, 0.0f }, minorBump ? 16 : 30,
                         minorBump ? ORANGE : RED, 3.0f, 12.0f, 0.28f, 0.45f);

    if (player_.nanotechHp <= 0.01f) {
        player_.nanotechHp = 0.0f;
        mode_ = ScreenMode::GAME_OVER;
        int distInt = static_cast<int>(distanceMeters_);
        save_.totalDistanceGrinded += distInt;
        if (isDailyRun_) {
            if (distInt > save_.bestDailyMeters) save_.bestDailyMeters = distInt;
        } else {
            if (distInt > save_.bestDistanceMeters) save_.bestDistanceMeters = distInt;
        }
        SaveProgress();
    }
}

void RailGame::SpawnRailBoltLine(int lane, float startZ, int count, float spacing, bool jumpArc) {
    float lx = GetLaneX(std::clamp(lane, 0, NUM_LANES - 1));
    int spawned = 0;
    for (int i = 0; i < count; ++i) {
        float bz = startZ + i * spacing;
        // Never spawn bolts past the end of a rail or on an inactive rail!
        if (!jumpArc && !IsRailActiveAt(lane, bz)) continue;
        int minL = GetCorridorMinLaneAt(totalWorldTravel_ + bz);
        if (lane < minL || lane > minL + 2) continue;

        for (auto& b : bolts_) {
            if (!b.active) {
                b.active = true;
                b.isRaritanium = false;
                b.isTitanium = (i == count / 2 && rng_.Chance(0.22f));
                b.onRail = true;
                float t = (count > 1) ? static_cast<float>(i) / static_cast<float>(count - 1) : 0.5f;
                float arcY = jumpArc ? (std::sin(t * 3.14159265f) * 2.45f) : 0.0f;
                b.pos = { lx, 0.78f + arcY, bz };
                b.vel = { 0.0f, 0.0f, 0.0f };
                b.value = b.isTitanium ? 5 : 1;
                b.spin = i * 42.0f;
                spawned++;
                break;
            }
        }
    }
    (void)spawned;
}

void RailGame::SpawnChunkPattern(float startZ) {
    float difficulty = std::min(1.0f, distanceMeters_ / 2200.0f);
    int pattern = rng_.RangeInt(0, 6);

    // Calculate active 3-lane route corridor at this continuous world distance (Strictly capped at 3 rails!)
    float worldZ = totalWorldTravel_ + startZ;
    int minLane = GetCorridorMinLaneAt(worldZ);
    std::array<int, 3> activeLanes = { minLane, minLane + 1, minLane + 2 };
    auto CrateOrRari = [&]() -> HazardKind {
        if (IsMutatorActive(DailyMutatorId::RARITANIUM_RUSH) && rng_.Chance(0.32f)) {
            return HazardKind::RARITANIUM_CACHE;
        }
        return HazardKind::BOLT_CRATE;
    };

    if (pattern == 0) {
        // Wave 0: Single Laser Gate Row with 1 Open Safe Lane + Guided Bolt Line
        int safeLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnRailBoltLine(safeLane, startZ + 2.0f, 5, 2.8f, false);
        for (int l : activeLanes) {
            if (l == safeLane) {
                SpawnHazard((player_.nanotechHp < player_.maxNanotechHp && rng_.Chance(0.45f))
                            ? HazardKind::NANOTECH_CRATE : CrateOrRari(), l, startZ + 18.0f);
            } else {
                bool isLow = IsMutatorActive(DailyMutatorId::AERIAL_ASSAULT) ? true : rng_.Chance(0.55f);
                SpawnHazard(isLow ? HazardKind::LOW_LASER_BARRIER : HazardKind::HIGH_LASER_GATE, l, startZ + 10.0f);
                if (isLow) {
                    SpawnRailBoltLine(l, startZ + 4.0f, 5, 2.8f, true);
                }
            }
        }
    } else if (pattern == 1) {
        // Wave 1: Cinematic Sheared Rail Chasm (18.0m gap!) + Hyper Boost Pad on safe lane
        int boostLane = activeLanes[rng_.RangeInt(0, 2)];
        int secondSafe = (distanceMeters_ < 250.0f) ? activeLanes[(rng_.RangeInt(0, 1) == 0) ? 0 : 2] : -1;
        if (secondSafe == boostLane) secondSafe = -1;

        for (int l : activeLanes) {
            if (l == boostLane) {
                SpawnHazard(HazardKind::HYPER_BOOST_PAD, l, startZ + 5.0f);
                SpawnRailBoltLine(l, startZ + 11.0f, 5, 2.8f, false);
                SpawnHazard(HazardKind::RARITANIUM_CACHE, l, startZ + 26.0f);
            } else if (l == secondSafe) {
                SpawnRailBoltLine(l, startZ + 6.0f, 5, 2.8f, false);
            } else {
                // Grand Sheared Chasm (18.0m!) with overhead Swingshot Versa-Fuse Node or Clank Gliding Arc
                SpawnHazard(HazardKind::BROKEN_RAIL_GAP, l, startZ + 14.0f, 18.0f);
                if (rng_.Chance(0.65f)) {
                    SpawnHazard(HazardKind::SWINGSHOT_NODE, l, startZ + 14.0f);
                } else {
                    SpawnRailBoltLine(l, startZ + 7.0f, 6, 2.8f, true); // High arching bolt arc for Clank gliding
                }
            }
        }
    } else if (pattern == 2) {
        // Wave 2: Thugs-4-Less Chopper Drone + Optional TNT Barrel (1 lane stays clear!)
        int clearLane = activeLanes[rng_.RangeInt(0, 2)];
        int droneLane = (activeLanes[0] == clearLane) ? activeLanes[1] : activeLanes[0];
        int tntLane   = (activeLanes[2] == clearLane) ? activeLanes[1] : activeLanes[2];
        SpawnRailBoltLine(clearLane, startZ + 4.0f, 6, 2.8f, false);
        SpawnHazard(HazardKind::THUG_DRONE, droneLane, startZ + 14.0f);
        SpawnHazard(HazardKind::TNT_CRATE, tntLane, startZ + 12.0f);
        SpawnHazard(CrateOrRari(), droneLane, startZ + 24.0f);
    } else if (pattern == 3) {
        // Wave 3: Single Clean Jump or Slide Line
        int openLane = activeLanes[rng_.RangeInt(0, 2)];
        for (int l : activeLanes) {
            if (l == openLane) {
                SpawnRailBoltLine(l, startZ + 4.0f, 5, 2.8f, false);
                SpawnHazard(CrateOrRari(), l, startZ + 20.0f);
            } else {
                bool isLow = IsMutatorActive(DailyMutatorId::AERIAL_ASSAULT) ? true : (l % 2 == 0);
                SpawnHazard(isLow ? HazardKind::LOW_LASER_BARRIER : HazardKind::HIGH_LASER_GATE, l, startZ + 11.0f);
                if (isLow) {
                    SpawnRailBoltLine(l, startZ + 5.5f, 5, 2.6f, true);
                }
            }
        }
    } else if (pattern == 4) {
        // Wave 4: Electrified Rail on 1 lane + Megacorp Enforcer
        int safeLane = activeLanes[rng_.RangeInt(0, 2)];
        int enfLane  = (activeLanes[0] == safeLane) ? activeLanes[1] : activeLanes[0];
        int elecLane = (activeLanes[2] == safeLane) ? activeLanes[1] : activeLanes[2];
        SpawnRailBoltLine(safeLane, startZ + 4.0f, 6, 2.8f, false);
        SpawnHazard(CrateOrRari(), safeLane, startZ + 22.0f);
        SpawnHazard(HazardKind::SHIELDED_ENFORCER, enfLane, startZ + 14.0f);
        SpawnHazard(HazardKind::ELECTRIFIED_RAIL, elecLane, startZ + 12.0f, 9.5f);
    } else if (pattern == 5) {
        // Wave 5: Pure Breather / Gadgetron Crate & Bolt Jackpot Straightaway (Bonus Raritanium on outer rails!)
        int rariLane = activeLanes[rng_.RangeInt(0, 2)];
        for (int l : activeLanes) {
            SpawnRailBoltLine(l, startZ + 2.0f, 5, 2.6f, false);
            if (l == rariLane) {
                SpawnHazard(rng_.Chance(0.55f) ? HazardKind::RARITANIUM_CACHE : HazardKind::NANOTECH_CRATE, l, startZ + 18.0f);
            } else {
                SpawnHazard(CrateOrRari(), l, startZ + 18.0f);
            }
        }
    } else {
        // Wave 6: Gentle Lane-Switch Guided by Bolts
        int open1 = activeLanes[rng_.RangeInt(0, 2)];
        int open2 = (activeLanes[0] == open1) ? activeLanes[1] : activeLanes[0];
        SpawnRailBoltLine(open1, startZ + 2.0f, 5, 2.6f, false);
        for (int l : activeLanes) {
            if (l != open1) SpawnHazard(HazardKind::ELECTRIFIED_RAIL, l, startZ + 8.0f, 7.5f);
        }
        SpawnRailBoltLine(open2, startZ + 22.0f, 5, 2.6f, false);
        SpawnHazard(CrateOrRari(), open2, startZ + 36.0f);
    }

    // Daily Mutator Track Composition Modifiers (always placed at startZ + 30m so primary lane stays fair!)
    if (IsMutatorActive(DailyMutatorId::ARMORED_CONVOY) && rng_.Chance(0.55f)) {
        int eLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(rng_.Chance(0.45f) ? HazardKind::SHIELDED_ENFORCER : HazardKind::THUG_DRONE, eLane, startZ + 31.0f);
    } else if (IsMutatorActive(DailyMutatorId::HIGH_VOLTAGE_GRID) && rng_.Chance(0.52f)) {
        int vLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::ELECTRIFIED_RAIL, vLane, startZ + 30.0f, 7.0f);
    } else if (IsMutatorActive(DailyMutatorId::VOLATILE_CARGO) && rng_.Chance(0.58f)) {
        int tLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::TNT_CRATE, tLane, startZ + 30.0f);
    } else if (IsMutatorActive(DailyMutatorId::OVERCLOCKED_RAILS) && rng_.Chance(0.48f)) {
        int bLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::HYPER_BOOST_PAD, bLane, startZ + 30.0f);
    }

    // Dynamic Sector-Specific Environmental Hazards
    GalaxySector sec = GetCurrentSector();
    if (IsMutatorActive(DailyMutatorId::RARITANIUM_RUSH) && rng_.Chance(0.45f)) {
        int xLane = activeLanes[rng_.RangeInt(0, 2)];
        HazardKind wild[4] = { HazardKind::HOVER_TAXI, HazardKind::MAGMA_GEYSER, HazardKind::PIRANHA_LEAP, HazardKind::SWINGING_CRANE_SCRAP };
        SpawnHazard(wild[rng_.RangeInt(0, 3)], xLane, startZ + 27.0f);
    } else if (sec == GalaxySector::CORSON_V && rng_.Chance(0.42f)) {
        int tLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::HOVER_TAXI, tLane, startZ + 28.0f);
    } else if (sec == GalaxySector::GASPAR_VOLCANIC && rng_.Chance(0.48f)) {
        int gLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::MAGMA_GEYSER, gLane, startZ + 26.0f);
    } else if (sec == GalaxySector::POKITARU_RESORT && rng_.Chance(0.48f)) {
        int pLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::PIRANHA_LEAP, pLane, startZ + 24.0f);
    } else if (sec == GalaxySector::TORREN_IV_SCRAP && rng_.Chance(0.42f)) {
        int cLane = activeLanes[rng_.RangeInt(0, 2)];
        SpawnHazard(HazardKind::SWINGING_CRANE_SCRAP, cLane, startZ + 27.0f);
    }

    // Generous open breathing room before the next obstacle chunk (62m early on -> 48m at max speed)!
    float spacing = std::max(48.0f, 62.0f - difficulty * 14.0f);
    nextChunkZ_ = startZ + spacing;
}

void RailGame::SpawnHazard(HazardKind kind, int lane, float z, float lengthZ) {
    // Strictly prevent obstacles, crates, mines, or enemies from spawning on inactive or ended rails!
    if (kind != HazardKind::BROKEN_RAIL_GAP && !IsRailActiveAt(lane, z)) return;
    if (kind == HazardKind::BROKEN_RAIL_GAP) {
        int minL = GetCorridorMinLaneAt(totalWorldTravel_ + z);
        if (lane < minL || lane > minL + 2) return;
    }

    for (auto& h : hazards_) {
        if (!h.active) {
            h.active = true;
            h.kind = kind;
            h.lane = std::clamp(lane, 0, NUM_LANES - 1);
            h.z = z;
            h.lengthZ = lengthZ;
            h.animPhase = rng_.RangeFloat(0.0f, 6.28f);
            h.shootTimer = rng_.RangeFloat(1.2f, 2.4f);

            if (kind == HazardKind::SHIELDED_ENFORCER) h.maxHp = 55.0f;
            else if (kind == HazardKind::HOVER_TAXI)  h.maxHp = 35.0f;
            else if (kind == HazardKind::THUG_DRONE)   h.maxHp = 22.0f;
            else if (kind == HazardKind::PIRANHA_LEAP) h.maxHp = 18.0f;
            else if (kind == HazardKind::RARITANIUM_CACHE) h.maxHp = 18.0f;
            else if (kind == HazardKind::MAGMA_GEYSER || kind == HazardKind::SWINGING_CRANE_SCRAP) h.maxHp = 999.0f;
            else h.maxHp = 10.0f;
            h.hp = h.maxHp;
            return;
        }
    }
}

void RailGame::SpawnBoltShower(Vector3 origin, int count, int raritaniumCount) {
    int spawned = 0;
    for (auto& b : bolts_) {
        if (!b.active) {
            b.active = true;
            b.isRaritanium = (spawned < raritaniumCount);
            b.isTitanium = (!b.isRaritanium && spawned == raritaniumCount && rng_.Chance(0.25f));
            b.onRail = false; // Scattered shower bolt -> immediately magnetized to Ratchet!
            b.pos = { origin.x + rng_.RangeFloat(-1.2f, 1.2f),
                      origin.y + rng_.RangeFloat(0.4f, 2.2f),
                      origin.z + rng_.RangeFloat(-1.5f, 1.5f) };
            b.vel = { rng_.RangeFloat(-4.5f, 4.5f), rng_.RangeFloat(4.5f, 9.5f), rng_.RangeFloat(-3.0f, 3.0f) };
            b.value = b.isRaritanium ? 1 : (b.isTitanium ? 5 : 1);
            b.spin = rng_.RangeFloat(0.0f, 360.0f);
            if (++spawned >= count) break;
        }
    }
}

void RailGame::SpawnProjectile(Vector3 pos, Vector3 vel, int lane, WeaponId wId, float dmg, float radius,
                               Color col, bool friendly, int bounces) {
    for (auto& p : projectiles_) {
        if (!p.active) {
            p.active = true;
            p.friendly = friendly;
            p.sourceWeapon = wId;
            p.lane = lane;
            p.pos = pos;
            p.vel = vel;
            p.damage = dmg;
            p.radius = radius;
            p.life = 2.2f;
            if (friendly) {
                switch (wId) {
                    case WeaponId::OMNIWRENCH_8000: p.maxRange = 44.0f; break;
                    case WeaponId::SHOCK_BLASTER:   p.maxRange = 40.0f; break;
                    case WeaponId::BUZZ_BLADES:     p.maxRange = 56.0f; break;
                    case WeaponId::COMBUSTER:       p.maxRange = 68.0f; break;
                    case WeaponId::RYNO_INFINITE:   p.maxRange = 88.0f; break;
                    case WeaponId::PLASMA_STRIKER:  p.maxRange = 128.0f; break;
                }
            } else {
                p.maxRange = 135.0f;
            }
            p.bouncesLeft = bounces;
            p.color = col;
            return;
        }
    }
}

void RailGame::SpawnParticleBurst3D(Vector3 pos, int count, Color color, float minSpd, float maxSpd, float size, float life) {
    int spawned = 0;
    for (auto& pt : particles_) {
        if (!pt.active) {
            pt.active = true;
            pt.pos = pos;
            float theta = rng_.RangeFloat(0.0f, 6.283185f);
            float phi = rng_.RangeFloat(-1.2f, 1.2f);
            float spd = rng_.RangeFloat(minSpd, maxSpd);
            pt.vel = { std::cos(theta) * std::cos(phi) * spd,
                       std::abs(std::sin(phi)) * spd + 1.5f,
                       std::sin(theta) * std::cos(phi) * spd };
            pt.size = size * rng_.RangeFloat(0.7f, 1.3f);
            pt.maxLife = life * rng_.RangeFloat(0.7f, 1.2f);
            pt.life = pt.maxLife;
            pt.color = color;
            if (++spawned >= count) break;
        }
    }
}

void RailGame::TriggerBossDropship() {
    boss_.active = true;
    boss_.bossTier++;
    int bossChoice = (boss_.bossTier - 1) % 4;
    if (bossChoice == 0) {
        boss_.bossType = BossType::PROG_TWINS;
        boss_.name = "PROG TWINS // COMMAND WARSHIP";
        TriggerRadioTransmission(CommSpeaker::CAPTAIN_QWARK, "CAPTAIN QWARK",
                                 "Ratchet! It's the Megacorp Warship! Watch out for their twin plasma carpet-bombing!");
    } else if (bossChoice == 1) {
        boss_.bossType = BossType::EMPEROR_NEFARIOUS_FRIGATE;
        boss_.name = "EMPEROR NEFARIOUS // DREAD-FRIGATE";
        TriggerRadioTransmission(CommSpeaker::DR_NEFARIOUS, "DR. NEFARIOUS",
                                 "LOMBAX! Did you think you could escape my Imperial Frigate?! LAWRENCE, DESTROY HIM!");
    } else if (bossChoice == 2) {
        boss_.bossType = BossType::CAPTAIN_SLAG_CORSAIR;
        boss_.name = "CAPTAIN SLAG // SKY-GALLEON";
        TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                 "Space Pirate Captain Slag incoming! Brace for staggered broadside cannon salvos!");
    } else {
        boss_.bossType = BossType::DR_NEFARIOUS_MECH;
        boss_.name = "BIOLITERATOR // NEFARIOUS DREAD-MECH";
        TriggerRadioTransmission(CommSpeaker::DR_NEFARIOUS, "DR. NEFARIOUS",
                                 "MY DREAD-MECH HAS LOCKED ONTO YOUR SIGNATURE, LOMBAX! DODGE THIS!");
    }
    boss_.z = 38.0f;
    boss_.hoverX = 0.0f;
    boss_.hoverY = 4.2f;
    boss_.phase = 0.0f;
    activeDare_.active = false; // Clear radio channel for Boss encounter
    boss_.maxTurretHp = 110.0f + boss_.bossTier * 45.0f;
    if (IsMutatorActive(DailyMutatorId::GADGETRON_OVERLOAD)) {
        boss_.maxTurretHp *= 1.35f;
    }
    boss_.turretHp = { 0.0f, boss_.maxTurretHp, boss_.maxTurretHp, boss_.maxTurretHp, 0.0f };
    boss_.telegraphLane = -1;
    boss_.telegraphTimer = 0.0f;
    boss_.attackCooldown = 2.0f;
    boss_.laserSweepX = 0.0f;
    boss_.laserSweepTimer = 0.0f;

    cameraShake_ = 0.55f;
    audio_.PlayExplosion();
}

void RailGame::HandleInput() {
    float frameDt = GetFrameTime();
    if (touchFeedbackJump_ > 0.0f) touchFeedbackJump_ -= frameDt;
    if (touchFeedbackFire_ > 0.0f) touchFeedbackFire_ -= frameDt;
    if (touchFeedbackBoost_ > 0.0f) touchFeedbackBoost_ -= frameDt;
    if (touchFeedbackPause_ > 0.0f) touchFeedbackPause_ -= frameDt;

    if (IsKeyPressed(KEY_M)) {
        audio_.ToggleMute();
    }

    if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
#if !defined(PLATFORM_ANDROID)
        ToggleBorderlessWindowed();
#else
        ToggleFullscreen();
#endif
    }

    if (IsGamepadAvailable(0)) {
        activeInputDevice_ = GetGamepadName(0);
        if (activeInputDevice_.empty()) activeInputDevice_ = "GAMEPAD CONTROLLER";
    } else if (GetTouchPointCount() > 0) {
        activeInputDevice_ = "TOUCH SCREEN";
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) ||
               IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        activeInputDevice_ = "KEYBOARD & MOUSE";
    }

    if (mode_ == ScreenMode::PLAYING) {
        // Music Jukebox Skip: N or Gamepad Right Stick Click
        if (IsKeyPressed(KEY_N) || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_THUMB))) {
            int nextIdx = audio_.CycleNextActionTrack();
            toast_.title = "NOW PLAYING:";
            toast_.subtitle = audio_.GetActionTrackName(nextIdx);
            toast_.timer = 2.8f;
            toast_.active = true;
            audio_.PlayUiClick();
        }

        // Collect new touch points & mouse clicks
        struct TouchEvent {
            Vector2 pos;
        };
        std::vector<TouchEvent> newPresses;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            newPresses.push_back({ GetVirtualMouse() });
        }

        int tc = GetTouchPointCount();
        for (int i = 0; i < tc; ++i) {
            int tid = GetTouchPointId(i);
            bool isNew = true;
            for (int p = 0; p < prevTouchCount_; ++p) {
                if (prevTouchIds_[p] == tid) { isNew = false; break; }
            }
            if (isNew) {
                newPresses.push_back({ ToVirtualCoords(GetTouchPosition(i)) });
            }
        }

        // 1. On-Screen Touch Pause Button (Top-Right, aligned with HUD pause button)
        Rectangle pauseBtn = { virtualWidth_ - 386.0f, 20.0f, 54.0f, 54.0f };
        Rectangle pauseHitbox = { pauseBtn.x - 16.0f, pauseBtn.y - 16.0f, pauseBtn.width + 32.0f, pauseBtn.height + 32.0f };

        bool pauseTriggered = (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P));
        for (const auto& np : newPresses) {
            if (CheckCollisionPointRec(np.pos, pauseHitbox)) {
                touchFeedbackPause_ = 0.25f;
                pauseTriggered = true;
                break;
            }
        }
        if (pauseTriggered) {
            mode_ = ScreenMode::PAUSED;
            audio_.PlayUiClick();
            return;
        }

        // Keyboard Lane Switching
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  SwitchLane(-1);
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) SwitchLane(+1);

        // Wrench Slide / Crouch
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_LEFT_CONTROL)) {
            TriggerWrenchSlide();
        }

        // Weapon Switching (1..6 or Q/E or Mouse Wheel)
        for (int k = 0; k < static_cast<int>(NUM_WEAPONS); ++k) {
            if (IsKeyPressed(KEY_ONE + k) && save_.unlockedWeapons[k]) {
                save_.equippedWeapon = k;
                audio_.PlayUiClick();
            }
        }
        if (IsKeyPressed(KEY_Q)) CycleEquippedWeapon(-1);
        if (IsKeyPressed(KEY_E)) CycleEquippedWeapon(+1);
        float wheel = GetMouseWheelMove();
        if (wheel > 0.1f) CycleEquippedWeapon(-1);
        if (wheel < -0.1f) CycleEquippedWeapon(+1);

        // Shooting (F, J, Enter)
        if (IsKeyDown(KEY_F) || IsKeyDown(KEY_J) || IsKeyDown(KEY_ENTER)) {
            TriggerFireWeapon();
        }

        // Gamepad Support (Xbox, PlayStation, Switch Pro, Android BT controllers)
        bool hasGamepad = IsGamepadAvailable(0);
        if (hasGamepad) {
            const char* gpName = GetGamepadName(0);
            if (gpName && gpName[0] != '\0') activeInputDevice_ = gpName;

            // D-Pad or Left Stick lane switching
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) SwitchLane(-1);
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) SwitchLane(+1);

            float stickX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            static bool stickCenteredX = true;
            if (stickCenteredX) {
                if (stickX < -0.55f) {
                    SwitchLane(-1);
                    stickCenteredX = false;
                } else if (stickX > 0.55f) {
                    SwitchLane(+1);
                    stickCenteredX = false;
                }
            } else {
                if (std::abs(stickX) < 0.25f) stickCenteredX = true;
            }

            // Slide / Crouch: D-Pad Down, B/Circle button, Left Stick Down flick
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
                IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
                TriggerWrenchSlide();
            }
            float stickY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            static bool stickCenteredY = true;
            if (stickCenteredY) {
                if (stickY > 0.65f) {
                    TriggerWrenchSlide();
                    stickCenteredY = false;
                }
            } else {
                if (std::abs(stickY) < 0.30f) stickCenteredY = true;
            }

            // Jump & Glide: A / Cross button
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                TriggerJumpOrGlide();
            }
            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                touchHoldingJump_ = true;
            }

            // Weapon Cycle: LB / RB
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
                CycleEquippedWeapon(-1);
            }
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                CycleEquippedWeapon(+1);
            }

            // Fire Weapon: X / Square or Right Trigger (RT)
            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
                IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.25f) {
                TriggerFireWeapon();
            }

            // Overdrive Hoverboots: Y / Triangle or Left Trigger (LT)
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) ||
                IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.35f) {
                TriggerOverdriveBoost();
            }

            // Pause: Start / Options
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                mode_ = ScreenMode::PAUSED;
                audio_.PlayUiClick();
            }
        }

        // 2. Action Buttons & Quick-Select Weapon Hotbar Hitboxes
        Rectangle btnJumpRec = save_.leftHandedControls
                             ? Rectangle{ 30.0f, 720.0f - 216.0f, 96.0f, 96.0f }
                             : Rectangle{ virtualWidth_ - 128.0f, 720.0f - 216.0f, 96.0f, 96.0f };
        Rectangle btnFireRec = save_.leftHandedControls
                             ? Rectangle{ 140.0f, 720.0f - 208.0f, 82.0f, 82.0f }
                             : Rectangle{ virtualWidth_ - 238.0f, 720.0f - 208.0f, 82.0f, 82.0f };
        Rectangle btnBoostRec = save_.leftHandedControls
                              ? Rectangle{ 236.0f, 720.0f - 200.0f, 74.0f, 74.0f }
                              : Rectangle{ virtualWidth_ - 326.0f, 720.0f - 200.0f, 74.0f, 74.0f };
        float pad = 14.0f;
        Rectangle jumpHitbox = { btnJumpRec.x - pad, btnJumpRec.y - pad, btnJumpRec.width + pad * 2.0f, btnJumpRec.height + pad * 2.0f };
        Rectangle fireHitbox = { btnFireRec.x - pad, btnFireRec.y - pad, btnFireRec.width + pad * 2.0f, btnFireRec.height + pad * 2.0f };
        Rectangle boostHitbox = { btnBoostRec.x - pad, btnBoostRec.y - pad, btnBoostRec.width + pad * 2.0f, btnBoostRec.height + pad * 2.0f };

        // Process Action Buttons
        for (const auto& np : newPresses) {
            // Weapon Hotbar tap
            for (int w = 0; w < static_cast<int>(NUM_WEAPONS); ++w) {
                Rectangle slotRect = { virtualWidth_ - 418.0f + w * 66.0f, 720.0f - 84.0f, 60.0f, 64.0f };
                Rectangle slotHitbox = { slotRect.x - 8.0f, slotRect.y - 8.0f, slotRect.width + 16.0f, slotRect.height + 16.0f };
                if (CheckCollisionPointRec(np.pos, slotHitbox) && save_.unlockedWeapons[w]) {
                    save_.equippedWeapon = w;
                    audio_.PlayUiClick();
                    break;
                }
            }

            if (CheckCollisionPointRec(np.pos, jumpHitbox)) {
                touchFeedbackJump_ = 0.25f;
                TriggerJumpOrGlide();
                touchHoldingJump_ = true;
            } else if (CheckCollisionPointRec(np.pos, fireHitbox)) {
                touchFeedbackFire_ = 0.25f;
                TriggerFireWeapon();
            } else if (save_.hoverbootsUnlocked && CheckCollisionPointRec(np.pos, boostHitbox)) {
                TriggerOverdriveBoost();
            }
        }

        // Screen Swipes & Continuous Air Hold
        Vector2 vm = GetVirtualMouse();
        bool pointerInActionArea = (CheckCollisionPointRec(vm, jumpHitbox) || CheckCollisionPointRec(vm, fireHitbox) ||
                                    (save_.hoverbootsUnlocked && CheckCollisionPointRec(vm, boostHitbox)) || vm.y > VIRTUAL_HEIGHT - 90.0f);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(vm, jumpHitbox)) {
            touchHoldingJump_ = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !pointerInActionArea) {
            pointerDown_ = true;
            pointerStartPos_ = vm;
            pointerDownTime_ = globalTime_;
            gestureConsumed_ = false;

            if (!player_.grounded && player_.heliFuel > 0.0f) {
                touchHoldingJump_ = true;
            }
        }

        if (pointerDown_ && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = Vector2Subtract(vm, pointerStartPos_);
            float dist = Vector2Length(delta);

            float swipeThresh = (save_.swipeSensitivity == 0) ? 36.0f : (save_.swipeSensitivity == 1) ? 26.0f : 18.0f;
            if (!gestureConsumed_ && dist > swipeThresh) {
                if (std::abs(delta.x) > std::abs(delta.y)) {
                    SwitchLane(delta.x < 0.0f ? -1 : +1);
                    gestureConsumed_ = true;
                } else {
                    if (delta.y < -24.0f) {
                        TriggerJumpOrGlide();
                        touchHoldingJump_ = true;
                        gestureConsumed_ = true;
                    } else if (delta.y > 28.0f) {
                        TriggerWrenchSlide();
                        gestureConsumed_ = true;
                    }
                }
            } else if (gestureConsumed_ && delta.y < -20.0f) {
                touchHoldingJump_ = true;
            }
        }

        if (pointerDown_ && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (!gestureConsumed_) {
                Vector2 delta = Vector2Subtract(vm, pointerStartPos_);
                if (Vector2Length(delta) < 24.0f && (globalTime_ - pointerDownTime_) < 0.35f) {
                    TriggerFireWeapon();
                }
            }
            pointerDown_ = false;
            gestureConsumed_ = false;
            touchHoldingJump_ = false;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && tc == 0) {
            touchHoldingJump_ = false;
        }

        // Jump & Heli-Pack Glide (Keyboard + Touch Holding)
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            TriggerJumpOrGlide();
        }
        bool jumpHeld = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || touchHoldingJump_;
        if (!player_.grounded && player_.heliFuel > 0.0f) {
            if (jumpHeld && player_.vy <= 6.5f) {
                player_.heliGliding = true;
            } else if (!jumpHeld) {
                player_.heliGliding = false;
            }
        } else {
            player_.heliGliding = false;
        }

        // Overdrive Hoverboots Boost (Shift / B)
        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || IsKeyPressed(KEY_B)) {
            TriggerOverdriveBoost();
        }
    } else if (mode_ == ScreenMode::SPLASH_INTRO) {
        splashTimer_ += GetFrameTime();
        if (splashTimer_ >= 2.4f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetTouchPointCount() > 0 || GetKeyPressed() > 0 ||
            (IsGamepadAvailable(0) && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))) {
            SetScreenMode(ScreenMode::TITLE_FRONT_PAGE);
        }
    } else if (mode_ == ScreenMode::TITLE_FRONT_PAGE) {
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            GetTouchPointCount() > 0 || GetKeyPressed() > 0 ||
            (IsGamepadAvailable(0) && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))) {
            SetScreenMode(ScreenMode::MAIN_MENU);
            audio_.PlayUiClick();
        }
    } else if (mode_ == ScreenMode::MAIN_MENU) {
        if (screenTransitionCooldown_ <= 0.0f) {
            bool hasGamepad = IsGamepadAvailable(0);
            // Toggle Standard / Daily with Left/Right or Tab
            if (IsKeyPressed(KEY_LEFT) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))) {
                menuSelectDaily_ = false;
                audio_.PlayUiClick();
            } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_TAB) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))) {
                menuSelectDaily_ = true;
                audio_.PlayUiClick();
            }
            // Launch Selected Mode: Space, Enter, or Gamepad (A) / Start
            else if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
                (hasGamepad && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))) {
                StartNewRun(menuSelectDaily_);
            }
            // Arsenal shortcut: A
            else if (IsKeyPressed(KEY_A)) {
                SetScreenMode(ScreenMode::GADGETRON_VENDOR);
                audio_.PlayUiClick();
            }
            // Daily Challenge Seed: X or Gamepad (X)
            else if (IsKeyPressed(KEY_X) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))) {
                StartNewRun(true);
            }
            // Gadgetron Vendor: V or Gamepad (Y)
            else if (IsKeyPressed(KEY_V) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))) {
                SetScreenMode(ScreenMode::GADGETRON_VENDOR);
                audio_.PlayUiClick();
            }
            // Raritanium Lab: L or Gamepad (LB)
            else if (IsKeyPressed(KEY_L) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))) {
                SetScreenMode(ScreenMode::RARITANIUM_LAB);
                audio_.PlayUiClick();
            }
            // Wardrobe: W or Gamepad (RB)
            else if (IsKeyPressed(KEY_W) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) {
                wardrobePreviewArmor_ = save_.equippedArmor;
                wardrobePreviewClank_ = save_.equippedClankSkin;
                wardrobeRotAngle_ = 180.0f;
                SetScreenMode(ScreenMode::ARMOR_WARDROBE);
                audio_.PlayUiClick();
            }
            // Skill Points: K
            else if (IsKeyPressed(KEY_K)) {
                SetScreenMode(ScreenMode::SKILL_POINTS_VIEW);
                audio_.PlayUiClick();
            }
            // Career Stats: C
            else if (IsKeyPressed(KEY_C)) {
                SetScreenMode(ScreenMode::STATS_VIEW);
                audio_.PlayUiClick();
            }
            // Settings: S or Gamepad Select/Back (MIDDLE_LEFT)
            else if (IsKeyPressed(KEY_S) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT))) {
                settingsPrevMode_ = ScreenMode::MAIN_MENU;
                SetScreenMode(ScreenMode::SETTINGS_VIEW);
                audio_.PlayUiClick();
            }
            // Back to Title: Escape or Gamepad (B)
            else if (IsKeyPressed(KEY_ESCAPE) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))) {
                SetScreenMode(ScreenMode::TITLE_FRONT_PAGE);
                audio_.PlayUiClick();
            }
        }
    } else if (mode_ == ScreenMode::PAUSED) {
        bool hasGamepad = IsGamepadAvailable(0);
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE) ||
            (hasGamepad && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))) {
            unpauseRampTimer_ = 0.75f;
            SetScreenMode(ScreenMode::PLAYING);
            audio_.PlayUiClick();
        } else if (IsKeyPressed(KEY_R)) {
            StartNewRun(isDailyRun_);
        } else if (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
            SaveProgress();
            SetScreenMode(ScreenMode::MAIN_MENU);
            audio_.PlayUiClick();
        }
    } else if (mode_ == ScreenMode::GAME_OVER) {
        bool hasGamepad = IsGamepadAvailable(0);
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_R) ||
            (hasGamepad && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)))) {
            StartNewRun(isDailyRun_);
        } else if (IsKeyPressed(KEY_A) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))) {
            SetScreenMode(ScreenMode::GADGETRON_VENDOR);
            audio_.PlayUiClick();
        } else if (IsKeyPressed(KEY_ESCAPE) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))) {
            SetScreenMode(ScreenMode::MAIN_MENU);
            audio_.PlayUiClick();
        }
    } else if (mode_ != ScreenMode::MAIN_MENU && mode_ != ScreenMode::TITLE_FRONT_PAGE && mode_ != ScreenMode::SPLASH_INTRO) {
        if (screenTransitionCooldown_ <= 0.0f) {
            bool hasGamepad = IsGamepadAvailable(0);
            bool padBack = hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
            int tabDelta = 0;
            if (IsKeyPressed(KEY_Q) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))) tabDelta = -1;
            else if (IsKeyPressed(KEY_E) || (hasGamepad && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) tabDelta = 1;

            if (IsKeyPressed(KEY_ESCAPE) || padBack) {
                if (mode_ == ScreenMode::SETTINGS_VIEW) {
                    if (showResetConfirm_) {
                        showResetConfirm_ = false;
                    } else {
                        SaveProgress();
                        SetScreenMode(settingsPrevMode_);
                    }
                } else {
                    SetScreenMode(ScreenMode::MAIN_MENU);
                }
                audio_.PlayUiClick();
            } else if (tabDelta != 0) {
                if (mode_ == ScreenMode::GADGETRON_VENDOR || mode_ == ScreenMode::RARITANIUM_LAB || mode_ == ScreenMode::ARMOR_WARDROBE) {
                    int curTab = (mode_ == ScreenMode::GADGETRON_VENDOR) ? 0 : (mode_ == ScreenMode::RARITANIUM_LAB ? 1 : 2);
                    int nextTab = (curTab + tabDelta + 3) % 3;
                    if (nextTab == 0) {
                        SetScreenMode(ScreenMode::GADGETRON_VENDOR);
                    } else if (nextTab == 1) {
                        SetScreenMode(ScreenMode::RARITANIUM_LAB);
                    } else {
                        wardrobePreviewArmor_ = save_.equippedArmor;
                        wardrobePreviewClank_ = save_.equippedClankSkin;
                        wardrobeRotAngle_ = 180.0f;
                        SetScreenMode(ScreenMode::ARMOR_WARDROBE);
                    }
                    audio_.PlayUiClick();
                } else if (mode_ == ScreenMode::BOUNTIES_VIEW || mode_ == ScreenMode::SKILL_POINTS_VIEW || mode_ == ScreenMode::STATS_VIEW) {
                    int curTab = (mode_ == ScreenMode::BOUNTIES_VIEW) ? 0 : (mode_ == ScreenMode::SKILL_POINTS_VIEW ? 1 : 2);
                    int nextTab = (curTab + tabDelta + 3) % 3;
                    if (nextTab == 0) SetScreenMode(ScreenMode::BOUNTIES_VIEW);
                    else if (nextTab == 1) SetScreenMode(ScreenMode::SKILL_POINTS_VIEW);
                    else SetScreenMode(ScreenMode::STATS_VIEW);
                    audio_.PlayUiClick();
                } else if (mode_ == ScreenMode::SETTINGS_VIEW) {
                    settingsTab_ = 1 - settingsTab_;
                    audio_.PlayUiClick();
                }
            }
        }
    }

    // Store active touch IDs for next frame
    int curTc = GetTouchPointCount();
    prevTouchCount_ = std::min(curTc, static_cast<int>(prevTouchIds_.size()));
    for (int i = 0; i < prevTouchCount_; ++i) {
        prevTouchIds_[i] = GetTouchPointId(i);
    }
}

void RailGame::UpdateFixed(float dt) {
    globalTime_ += dt;

    if (cameraShake_ > 0.0f) cameraShake_ = std::max(0.0f, cameraShake_ - 1.8f * dt);
    if (screenFlashAlpha_ > 0.0f) screenFlashAlpha_ = std::max(0.0f, screenFlashAlpha_ - 1.6f * dt);
    if (toast_.active) {
        toast_.timer -= dt;
        if (toast_.timer <= 0.0f) toast_.active = false;
    }

    if (mode_ == ScreenMode::ARMOR_WARDROBE) {
        // Drag to rotate 3D character in showcase
        Vector2 m = GetVirtualMouse();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && m.x < 560.0f && m.y > 115.0f && m.y < 650.0f) {
            if (!isDraggingWardrobe_) {
                isDraggingWardrobe_ = true;
                lastMousePos_ = m;
            } else {
                float dx = m.x - lastMousePos_.x;
                wardrobeRotAngle_ -= dx * 0.65f;
                lastMousePos_ = m;
            }
        } else {
            isDraggingWardrobe_ = false;
            // Idle auto-rotation
            wardrobeRotAngle_ += dt * 24.0f;
        }

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  wardrobeRotAngle_ -= 80.0f * dt;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) wardrobeRotAngle_ += 80.0f * dt;
    }

    // Scroll background skyline buildings
    float scrollSpd = (mode_ == ScreenMode::PLAYING) ? grindSpeed_ : 14.0f;
    for (auto& b : buildings_) {
        b.pos.z -= scrollSpd * 0.85f * dt;
        if (b.pos.z < -30.0f) {
            b.pos.z += static_cast<float>(MAX_SKY_BUILDINGS / 2) * 14.0f;
        }
    }

    UpdateParticlesFixed(dt);

    if (mode_ == ScreenMode::PLAYING) {
        UpdatePlayingFixed(dt);
    }
}

void RailGame::UpdatePlayingFixed(float dt) {
    runTime_ += dt;

    float baseSpd = (isDailyRun_ ? 42.0f : 38.0f) + std::min(28.0f, distanceMeters_ * 0.012f);
    if (IsMutatorActive(DailyMutatorId::OVERCLOCKED_RAILS)) {
        baseSpd *= 1.12f;
    }
    if (player_.hyperBoostTimer > 0.0f) {
        player_.hyperBoostTimer = std::max(0.0f, player_.hyperBoostTimer - dt);
        baseSpd *= 1.55f;
    }
    grindSpeed_ = Lerp(grindSpeed_, baseSpd, 4.0f * dt);

    float stepDist = grindSpeed_ * dt;
    totalWorldTravel_ += stepDist;
    float dMeters = stepDist * 0.45f;
    distanceMeters_ += dMeters;
    distanceSinceBoss_ += dMeters;
    runDistanceCarry_ += dMeters;
    if (runDistanceCarry_ >= 10.0f) {
        int dm = static_cast<int>(runDistanceCarry_);
        runDistanceCarry_ -= static_cast<float>(dm);
        ProgressContract(6, dm);
    }

    if (activeDare_.active) {
        activeDare_.timer -= dt;
        if (activeDare_.dareType == 1) {
            int cleanMeters = static_cast<int>(std::max(0.0f, distanceMeters_ - activeDare_.startDistance));
            activeDare_.progress = std::min(activeDare_.target, cleanMeters);
            if (activeDare_.progress >= activeDare_.target) {
                AdvanceRadioDare(1, 0);
            }
        }
        if (activeDare_.active && activeDare_.timer <= 0.0f) {
            activeDare_.active = false;
        }
    } else if (!boss_.active && sectorBannerTimer_ <= 0.0f && !radioComm_.active && distanceMeters_ >= nextDareDistance_) {
        nextDareDistance_ = distanceMeters_ + 460.0f;
        TriggerLiveRadioDare();
    }

    if (distanceMeters_ >= 500.0f) AwardSkillPoint(0);  // GRIND BOOT INITIATE
    if (distanceMeters_ >= 1500.0f) AwardSkillPoint(7); // DIMENSION HOPPER

    // Sector Transition Check
    GalaxySector curSec = GetCurrentSector();
    if (curSec != lastSector_) {
        lastSector_ = curSec;
        sectorBannerTimer_ = 3.5f;
        GrantWeaponAmmo(50, true);
        ProgressContract(11, 1);
        if (isDailyRun_) {
            save_.raritanium += 2;
            save_.totalRaritaniumEarnedLifetime += 2;
            runRaritaniumEarned_ += 2;
            audio_.PlayRaritaniumPickup();
            toast_.active = true;
            toast_.title = "DAILY SECTOR CLEARED!";
            toast_.subtitle = "+2 Bonus Raritanium & +50% Weapon Ammo Restocked";
            toast_.timer = 3.2f;
        }
        screenFlashColor_ = (curSec == GalaxySector::CORSON_V || curSec == GalaxySector::DIMENSIONATOR_RIFT) ? MAGENTA
                          : (curSec == GalaxySector::GASPAR_VOLCANIC || curSec == GalaxySector::TORREN_IV_SCRAP) ? ORANGE
                          : (curSec == GalaxySector::ZORDOOM_PRISON) ? RED : SKYBLUE;
        screenFlashAlpha_ = 0.14f;

        switch (curSec) {
            case GalaxySector::METROPOLIS_KERWAN:
                TriggerRadioTransmission(CommSpeaker::CAPTAIN_QWARK, "CAPTAIN QWARK",
                                         "Sector 1 Kerwan! Stand proud, citizens! Qwark is on patrol (from my penthouse)!");
                break;
            case GalaxySector::CORSON_V:
                TriggerRadioTransmission(CommSpeaker::DR_NEFARIOUS, "DR. NEFARIOUS",
                                         "Attention Nefarious City: Joy and Lombaxes remain strictly illegal! CEASE AT ONCE!");
                break;
            case GalaxySector::GASPAR_VOLCANIC:
                TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                         "Geothermal warnings active on Gaspar, Ratchet. Beware the erupting magma geysers!");
                break;
            case GalaxySector::POKITARU_RESORT:
                TriggerRadioTransmission(CommSpeaker::CAPTAIN_QWARK, "CAPTAIN QWARK",
                                         "Pokitaru Resort! The azure waters, the tropical breeze... the leaping cyber-piranhas!");
                break;
            case GalaxySector::TORREN_IV_SCRAP:
                TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                         "Molonoth scrap canyon ahead. Watch out for magnetic crane gantries swinging debris!");
                break;
            case GalaxySector::MEGACORP_FACILITY:
                TriggerRadioTransmission(CommSpeaker::CAPTAIN_QWARK, "CAPTAIN QWARK",
                                         "Megacorp Weapons Park! Remember, hero warranty is void if vaporized on the rails!");
                break;
            case GalaxySector::ZORDOOM_PRISON:
                TriggerRadioTransmission(CommSpeaker::DR_NEFARIOUS, "DR. NEFARIOUS",
                                         "Zordoom Maximum Security! No one escapes my penitentiary, especially not Lombaxes!");
                break;
            case GalaxySector::DIMENSIONATOR_RIFT:
                TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                         "Dimensional rift anomaly detected! Space-time is warping across all grind rails!");
                break;
        }
    }
    if (sectorBannerTimer_ > 0.0f) sectorBannerTimer_ -= dt;

    if (radioComm_.active) {
        radioComm_.timer -= dt;
        if (radioComm_.timer <= 0.0f) radioComm_.active = false;
    }

    // Overdrive Hoverboots Boost Mechanics
    if (player_.overdriveDuration > 0.0f) {
        player_.overdriveDuration = std::max(0.0f, player_.overdriveDuration - dt);
        player_.invulnTimer = std::max(player_.invulnTimer, 0.2f);
        player_.hyperBoostTimer = std::max(player_.hyperBoostTimer, 0.2f);
        if (rng_.Chance(0.85f)) {
            SpawnParticleBurst3D({ player_.x - 0.25f, player_.y + 0.1f, -0.6f }, 3, ORANGE, 2.0f, 6.0f, 0.22f, 0.25f);
            SpawnParticleBurst3D({ player_.x + 0.25f, player_.y + 0.1f, -0.6f }, 3, ORANGE, 2.0f, 6.0f, 0.22f, 0.25f);
            SpawnParticleBurst3D({ player_.x, player_.y + 0.1f, -0.6f }, 2, GOLD, 1.5f, 5.0f, 0.18f, 0.2f);
        }
    }
    if (player_.overdriveCooldown > 0.0f) {
        player_.overdriveCooldown = std::max(0.0f, player_.overdriveCooldown - dt);
    }

    // Swingshot Energy Grapple Cinematic Arc
    if (player_.isSwinging) {
        player_.swingTimer += dt;
        float st = std::clamp(player_.swingTimer / 0.72f, 0.0f, 1.0f);
        float targetX = GetLaneX(player_.swingTargetLane);
        player_.x = Lerp(player_.swingStartX, targetX, st);
        player_.y = 0.4f + std::sin(st * 3.14159f) * 3.5f;
        player_.vy = 0.0f;
        player_.grounded = false;

        if (rng_.Chance(0.75f)) {
            SpawnParticleBurst3D(player_.swingNodePos, 3, GREEN, 1.0f, 4.0f, 0.20f, 0.25f);
        }

        if (st >= 1.0f) {
            player_.isSwinging = false;
            player_.currentLane = player_.swingTargetLane;
            player_.x = targetX;
            player_.y = 0.0f;
            player_.grounded = true;
            player_.usedAirBoost = false;
            player_.landingSquashTimer = 0.25f;
            cameraShake_ = 0.20f;
            audio_.PlayRailHop();
            SpawnParticleBurst3D({ targetX, 0.1f, 0.0f }, 18, GREEN, 2.5f, 7.5f, 0.22f, 0.35f);
        }
    }

    // Trigger Boss Encounter every 750m (or 500m in Deadlocked Gauntlet!)
    float bossInterval = IsMutatorActive(DailyMutatorId::DEADLOCKED_GAUNTLET) ? 500.0f : 750.0f;
    if (!boss_.active && distanceSinceBoss_ >= bossInterval) {
        TriggerBossDropship();
    }

    if (player_.laneSwitchCooldown > 0.0f) {
        player_.laneSwitchCooldown = std::max(0.0f, player_.laneSwitchCooldown - dt);
    }
    if (player_.bufferedLaneTimer > 0.0f) {
        player_.bufferedLaneTimer = std::max(0.0f, player_.bufferedLaneTimer - dt);
        if (player_.bufferedLaneTimer <= 0.0f) player_.bufferedLaneDelta = 0;
    }

    // Player Horizontal Lane Interpolation & Acrobatic Rail-to-Rail Leap
    if (player_.isHopping) {
        player_.hopTimer += dt;
        float t = std::clamp(player_.hopTimer / player_.hopDuration, 0.0f, 1.0f);
        // Smooth hermite / smoothstep interpolation
        float smoothT = t * t * (3.0f - 2.0f * t);
        player_.x = Lerp(player_.hopStartX, player_.hopTargetX, smoothT);

        // Dynamic acrobatic tilt & roll
        player_.laneTiltDeg = -player_.hopDir * std::sin(t * 3.14159265f) * 40.0f;
        player_.hopRollDeg = player_.hopDir * 360.0f * t;

        if (t >= 1.0f) {
            player_.isHopping = false;
            player_.hopRollDeg = 0.0f;
            player_.laneSwitchCooldown = 0.05f; // Brief magnetic rail lock settle
            player_.landingSquashTimer = 0.22f; // Knee squash impact
            cameraShake_ = std::max(cameraShake_, 0.14f);
            audio_.PlayRailHop();

            // Reclamp magnetic sparks on destination rail
            SpawnParticleBurst3D({ player_.hopTargetX, player_.y + 0.08f, 0.0f }, 16, GOLD, 2.5f, 8.0f, 0.22f, 0.32f);
            SpawnParticleBurst3D({ player_.hopTargetX, player_.y + 0.08f, 0.0f }, 8, SKYBLUE, 1.5f, 5.0f, 0.15f, 0.22f);
        }
    } else if (player_.laneSwitchCooldown <= 0.0f && player_.bufferedLaneDelta != 0) {
        int queuedDelta = player_.bufferedLaneDelta;
        player_.bufferedLaneDelta = 0;
        player_.bufferedLaneTimer = 0.0f;
        SwitchLane(queuedDelta);
    } else {
        float targetX = GetLaneX(player_.currentLane);
        player_.x = Lerp(player_.x, targetX, 16.0f * dt);
        player_.laneTiltDeg = Lerp(player_.laneTiltDeg, 0.0f, 10.0f * dt);
    }

    // Check if the rail beneath Ratchet has terminated or broken
    if (player_.grounded && !player_.isHopping) {
        if (!IsRailActiveAt(player_.currentLane, 0.0f)) {
            // Rail ran out beneath Ratchet!
            // Clank immediately activates Emergency Auto-Hover for 0.5 seconds!
            player_.grounded = false;
            player_.emergencyHoverTimer = 0.5f;
            player_.heliGliding = true;
            player_.heliDeploy = 1.0f;
            player_.vy = 1.8f;
            player_.y = 0.12f;
            audio_.PlayJump();
            SpawnParticleBurst3D({ player_.x, 0.25f, -0.2f }, 16, SKYBLUE, 2.0f, 6.0f, 0.2f, 0.3f);
        }
    }

    // Emergency Auto-Hover state tracking
    if (player_.emergencyHoverTimer > 0.0f) {
        player_.emergencyHoverTimer -= dt;
        player_.heliGliding = true;
        player_.heliDeploy = 1.0f;
        // Hold altitude stably floating above the abyss
        if (player_.vy < -1.0f) player_.vy = -1.0f;
        if (player_.y < 0.05f) player_.y = 0.05f;

        if (player_.emergencyHoverTimer <= 0.0f) {
            // Emergency hover expired without reaching an active rail -> Ratchet falls to his death!
            player_.heliGliding = false;
            player_.heliDeploy = 0.0f;
            player_.vy = -26.0f; // Rapid plunge into the bottomless abyss
            audio_.PlayExplosion();
            cameraShake_ = 0.45f;
            screenFlashColor_ = RED;
            screenFlashAlpha_ = 0.35f;
        }
    }

    // Player Timers & Procedural Animation Counters
    if (player_.slideTimer > 0.0f) player_.slideTimer = std::max(0.0f, player_.slideTimer - dt);
    if (player_.wrenchSwingTimer > 0.0f) player_.wrenchSwingTimer = std::max(0.0f, player_.wrenchSwingTimer - dt);
    if (player_.weaponCooldown > 0.0f) player_.weaponCooldown = std::max(0.0f, player_.weaponCooldown - dt);
    if (player_.gunAimTimer > 0.0f) player_.gunAimTimer = std::max(0.0f, player_.gunAimTimer - dt);
    if (player_.gunRecoilTimer > 0.0f) player_.gunRecoilTimer = std::max(0.0f, player_.gunRecoilTimer - dt);
    if (player_.landingSquashTimer > 0.0f) player_.landingSquashTimer = std::max(0.0f, player_.landingSquashTimer - dt);
    if (player_.invulnTimer > 0.0f) player_.invulnTimer = std::max(0.0f, player_.invulnTimer - dt);
    player_.barrelSpinAngle += ((player_.gunAimTimer > 0.0f) ? 720.0f : 140.0f) * dt;

    // Vertical Jump & Clank Heli-Pack Physics
    if (!player_.grounded) {
        player_.jumpTime += dt;
        float grav = player_.heliGliding ? 11.0f : 44.0f;
        if (player_.heliGliding && player_.emergencyHoverTimer <= 0.0f) {
            player_.heliFuel = std::max(0.0f, player_.heliFuel - dt);
            if (player_.heliFuel <= 0.0f) player_.heliGliding = false;
        } else if (IsMutatorActive(DailyMutatorId::AERIAL_ASSAULT)) {
            player_.heliFuel = std::min(player_.maxHeliFuel, player_.heliFuel + 0.38f * dt);
        }
        player_.vy -= grav * dt;
        player_.y += player_.vy * dt;

        constexpr float MAX_PLAYER_HEIGHT = 4.65f;
        if (player_.y > MAX_PLAYER_HEIGHT) {
            player_.y = MAX_PLAYER_HEIGHT;
            if (player_.vy > 0.0f) player_.vy = 0.0f;
        }

        if (player_.y <= 0.0f) {
            if (IsRailActiveAt(player_.currentLane, 0.0f)) {
                // Safe landing on active rail!
                player_.y = 0.0f;
                player_.vy = 0.0f;
                player_.grounded = true;
                player_.heliGliding = false;
                player_.usedAirBoost = false;
                player_.emergencyHoverTimer = 0.0f;
                player_.heliFuel = player_.maxHeliFuel;
                player_.jumpTime = 0.0f;
                player_.landingSquashTimer = 0.25f;
                audio_.PlayRailHop();
                SpawnParticleBurst3D({ player_.x, 0.1f, 0.0f }, 10, GOLD, 2.0f, 7.0f, 0.18f, 0.25f);
            } else if (player_.emergencyHoverTimer <= 0.0f) {
                // Falling past rail level over empty chasm
                player_.grounded = false;
                if (player_.y <= -7.0f) {
                    DamagePlayer(player_.maxNanotechHp, false); // Fell to death in the abyss!
                }
            }
        }
    } else {
        player_.jumpTime = 0.0f;
        // Continuous Grind Boot sparks (tailored to equipped Armor suit!)
        if (rng_.Chance(0.65f)) {
            Color sparkCol = (player_.hyperBoostTimer > 0.0f) ? GREEN : ORANGE;
            Color sparkAlt = YELLOW;
            if (player_.hyperBoostTimer <= 0.0f) {
                if (save_.equippedArmor == static_cast<int>(ArmorSuitId::CARBONOX)) { // Carbonox: Cyan Electric Arcs
                    sparkCol = Color{ 40, 220, 255, 255 };
                    sparkAlt = SKYBLUE;
                } else if (save_.equippedArmor == static_cast<int>(ArmorSuitId::REBEL_LOMBAX)) { // Rebel Lombax: Cyan & Violet Motes
                    sparkCol = Color{ 60, 215, 255, 255 };
                    sparkAlt = Color{ 200, 110, 255, 255 };
                } else if (save_.equippedArmor == static_cast<int>(ArmorSuitId::INFERNOX)) { // Infernox: Volcanic Magma Embers
                    sparkCol = Color{ 255, 75, 20, 255 };
                    sparkAlt = RED;
                } else if (save_.equippedArmor == static_cast<int>(ArmorSuitId::DEADLOCKED_MARAUDER)) { // Deadlocked Marauder: Combat Crimson Sparks
                    sparkCol = Color{ 255, 30, 40, 255 };
                    sparkAlt = ORANGE;
                } else if (save_.equippedArmor == static_cast<int>(ArmorSuitId::LOMBAX_PRAETORIAN)) { // Lombax Praetorian: Royal Violet & Gold
                    sparkCol = Color{ 210, 110, 255, 255 };
                    sparkAlt = GOLD;
                }
            }
            SpawnParticleBurst3D({ player_.x - 0.18f, 0.08f, -0.2f }, 1, sparkCol, 2.0f, 6.0f, 0.16f, 0.22f);
            SpawnParticleBurst3D({ player_.x + 0.18f, 0.08f, +0.2f }, 1, sparkAlt, 2.0f, 6.0f, 0.16f, 0.22f);
        }
    }

    // Procedural Clank Heli-Pack Deployment & Spin Animation (NO Pop-in!)
    if (player_.heliGliding) {
        player_.heliDeploy = std::min(1.0f, player_.heliDeploy + dt * 3.6f); // Smooth mechanical unfolding in ~0.28s
        player_.heliRotorAngle += dt * (22.0f + 32.0f * player_.heliDeploy);
        if (player_.heliDeploy > 0.25f && rng_.Chance(0.50f)) {
            SpawnParticleBurst3D({ player_.x + rng_.RangeFloat(-0.10f, 0.10f), player_.y + 0.55f, -0.28f },
                                 2, SKYBLUE, 1.2f, 4.0f, 0.12f, 0.22f);
        }
    } else {
        player_.heliDeploy = std::max(0.0f, player_.heliDeploy - dt * 3.0f); // Smooth mechanical retraction in ~0.33s
        if (player_.heliDeploy > 0.01f) {
            player_.heliRotorAngle += dt * 18.0f * player_.heliDeploy;
        }
    }

    // Dynamic 3D Camera Follow (framing Ratchet low-center and the curving rails & reactor portal ahead)
    camera_.position.x = Lerp(camera_.position.x, player_.x * 0.72f, 8.0f * dt);
    camera_.position.y = Lerp(camera_.position.y, 3.25f + player_.y * 0.35f, 8.0f * dt);
    camera_.position.z = -6.8f;
    camera_.target.x = Lerp(camera_.target.x, player_.x * 0.55f + GetTrackCurveX(28.0f) * 0.35f, 10.0f * dt);
    camera_.target.y = Lerp(camera_.target.y, 2.25f + player_.y * 0.20f + GetTrackCurveY(28.0f) * 0.25f, 8.0f * dt);
    camera_.target.z = 26.0f;
    camera_.fovy = Lerp(camera_.fovy, (player_.hyperBoostTimer > 0.0f) ? 62.0f : 52.0f, 6.0f * dt);

    // Spawn ahead chunks well behind the reactor tunnel portal (z = 148) so nothing ever pops in!
    nextChunkZ_ -= stepDist;
    while (nextChunkZ_ < 175.0f) {
        SpawnChunkPattern(nextChunkZ_);
    }

    // Helper Lambda for Detonating TNT Barrels (Chain Reaction!)
    auto DetonateTntAt = [&](int tntLane, float tntZ) {
        audio_.PlayExplosion();
        cameraShake_ = 0.65f;
        ProgressContract(4, 1);
        AdvanceRadioDare(4, 6);
        SpawnParticleBurst3D({ GetLaneX(tntLane), 1.2f, tntZ }, 45, ORANGE, 5.0f, 18.0f, 0.40f, 0.65f);
        bool killedSomething = false;
        float blastZ = IsMutatorActive(DailyMutatorId::VOLATILE_CARGO) ? 16.0f : 9.5f;
        int baseBolts = IsMutatorActive(DailyMutatorId::VOLATILE_CARGO) ? 10 : 5;
        for (auto& other : hazards_) {
            if (!other.active) continue;
            if (std::abs(other.z - tntZ) < blastZ) {
                if (other.kind == HazardKind::BOLT_CRATE || other.kind == HazardKind::RARITANIUM_CACHE ||
                    other.kind == HazardKind::NANOTECH_CRATE ||
                    other.kind == HazardKind::THUG_DRONE || other.kind == HazardKind::SHIELDED_ENFORCER) {
                    other.active = false;
                    killedSomething = true;
                    bool othEnemy = (other.kind == HazardKind::THUG_DRONE || other.kind == HazardKind::SHIELDED_ENFORCER);
                    if (othEnemy) {
                        ProgressContract(2, 1);
                        AdvanceRadioDare(2, 1);
                    } else {
                        ProgressContract(0, 1);
                    }
                    int othRari = (other.kind == HazardKind::RARITANIUM_CACHE)
                                ? ((isDailyRun_ || distanceMeters_ >= 750.0f) ? 2 : 1)
                                : ((othEnemy && rng_.Chance(0.20f)) ? 1 : 0);
                    SpawnBoltShower({ GetLaneX(other.lane), 1.2f, other.z }, baseBolts + othRari, othRari);
                }
            }
        }
        if (killedSomething) {
            AwardSkillPoint(3); // EXPLOSIVE DEMOLITION
        }
    };

    // Update Hazards
    for (auto& h : hazards_) {
        if (!h.active) continue;
        h.z -= stepDist;
        h.animPhase += 3.5f * dt;

        // Clean up hazard when safely behind the camera
        if (h.z < -10.0f) {
            h.active = false;
            continue;
        }

        // Thug Drone shoots enemy plasma back down the lane
        if (h.kind == HazardKind::THUG_DRONE && h.z > 8.0f && h.z < 65.0f) {
            h.shootTimer -= dt;
            if (h.shootTimer <= 0.0f) {
                h.shootTimer = 2.4f;
                SpawnProjectile({ GetLaneX(h.lane), 1.3f, h.z - 1.0f }, { 0.0f, 0.0f, -26.0f },
                                h.lane, WeaponId::COMBUSTER, 1.0f, 0.55f, RED, false, 0);
            }
        }

        // Active OmniWrench Forward Melee Strike Reach (strictly in front of Ratchet on his rail!)
        bool wrenchActive = (player_.wrenchSwingTimer > 0.0f);
        float wrenchReachZ = 6.2f + save_.weaponLevels[0] * 0.55f;
        if (wrenchActive && player_.y < 2.4f &&
            h.lane == player_.currentLane && std::abs(player_.x - GetLaneX(h.lane)) < 1.55f &&
            h.z >= -0.5f && h.z < wrenchReachZ) {
            if (h.kind == HazardKind::BOLT_CRATE || h.kind == HazardKind::RARITANIUM_CACHE ||
                h.kind == HazardKind::NANOTECH_CRATE || h.kind == HazardKind::THUG_DRONE ||
                h.kind == HazardKind::SHIELDED_ENFORCER) {
                bool isRari = (h.kind == HazardKind::RARITANIUM_CACHE);
                bool isNano = (h.kind == HazardKind::NANOTECH_CRATE);
                bool isEnemy = (h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER);
                h.active = false;
                audio_.PlayWrenchSmash();
                AdvanceMultiplier();
                AddWeaponXp(0, isEnemy ? 20 : 12);
                ProgressContract(1, 1);
                AdvanceRadioDare(0, 1);
                AdvanceRadioDare(4, 1);
                if (isEnemy) {
                    runKills_++;
                    save_.totalEnemiesDestroyed++;
                    ProgressContract(2, 1);
                    AdvanceRadioDare(2, 1);
                    if (runKills_ >= 15) AwardSkillPoint(4);
                } else {
                    runCrates_++;
                    save_.totalCratesSmashed++;
                    ProgressContract(0, 1);
                    GrantWeaponAmmo(IsMutatorActive(DailyMutatorId::ARMORED_CONVOY) ? 50 : 30, false);
                    if (runCrates_ >= 25) AwardSkillPoint(1); // WRENCH NINJA
                    if (isRari) AwardSkillPoint(8);           // RARITANIUM MINER
                }
                if (isNano) {
                    player_.nanotechHp = std::min(player_.maxNanotechHp, player_.nanotechHp + 1.0f);
                    audio_.PlayLevelUp();
                    screenFlashColor_ = SKYBLUE;
                    screenFlashAlpha_ = 0.25f;
                }
                SpawnParticleBurst3D({ GetLaneX(h.lane), 1.0f, h.z }, 22,
                                     isEnemy ? ORANGE : ((isRari || isNano) ? SKYBLUE : GOLD), 3.0f, 11.0f, 0.26f, 0.4f);
                // OmniWrench melee smash grants +2 bonus Bolts & 25% Raritanium chance on enemies (2x cache yield past 750m / Daily)!
                int rariDrop = isRari
                             ? ((isDailyRun_ || distanceMeters_ >= 750.0f) ? 2 : 1)
                             : ((isEnemy && rng_.Chance(0.25f)) ? 1 : 0);
                SpawnBoltShower({ GetLaneX(h.lane), 1.0f, h.z }, isRari ? 10 : (isEnemy ? 9 : 7), rariDrop);
                continue;
            }
        }

        // Swingshot Versa-Fuse Grapple detection ahead of player (Z = 2.0m to 12.0m)
        if (h.kind == HazardKind::SWINGSHOT_NODE && !player_.isSwinging &&
            h.lane == player_.currentLane && h.z >= 2.0f && h.z <= 12.0f) {
            h.active = false;
            TriggerSwingshotGrapple({ GetLaneX(h.lane), 3.8f, h.z }, h.lane);
            continue;
        }

        // Check Direct Body Collision with Player (around Z = 0)
        float halfLen = h.lengthZ * 0.5f;
        if (h.lane == player_.currentLane && std::abs(player_.x - GetLaneX(h.lane)) < 1.35f &&
            (h.z - halfLen) < 0.9f && (h.z + halfLen) > -0.9f) {

            // Overdrive Hoverboots & Hyper-Boost Plow-Through (obliterates crates, gates & drones!)
            if (player_.overdriveDuration > 0.0f || player_.hyperBoostTimer > 0.0f) {
                if (h.kind != HazardKind::HYPER_BOOST_PAD && h.kind != HazardKind::BROKEN_RAIL_GAP && h.kind != HazardKind::SWINGSHOT_NODE) {
                    h.active = false;
                    audio_.PlayExplosion();
                    cameraShake_ = std::max(cameraShake_, 0.35f);
                    SpawnParticleBurst3D({ GetLaneX(h.lane), 1.2f, h.z }, 24, GOLD, 4.0f, 14.0f, 0.30f, 0.40f);
                    bool boostEnemy = (h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER || h.kind == HazardKind::HOVER_TAXI);
                    int boostRari = (h.kind == HazardKind::RARITANIUM_CACHE)
                                  ? ((isDailyRun_ || distanceMeters_ >= 750.0f) ? 2 : 1)
                                  : ((boostEnemy && rng_.Chance(0.20f)) ? 1 : 0);
                    SpawnBoltShower({ GetLaneX(h.lane), 1.2f, h.z }, 7 + boostRari, boostRari);
                    if (h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER || h.kind == HazardKind::HOVER_TAXI) {
                        runKills_++;
                        save_.totalEnemiesDestroyed++;
                    } else if (h.kind == HazardKind::BOLT_CRATE || h.kind == HazardKind::RARITANIUM_CACHE || h.kind == HazardKind::NANOTECH_CRATE) {
                        runCrates_++;
                        save_.totalCratesSmashed++;
                        GrantWeaponAmmo(30, false);
                    }
                    continue;
                }
            }

            if (h.kind == HazardKind::HYPER_BOOST_PAD) {
                if (player_.y < 1.5f) {
                    h.active = false;
                    player_.hyperBoostTimer = 2.6f + save_.bootThrusterLevel * 0.3f;
                    ProgressContract(10, 1);
                    audio_.PlayHyperBoost();
                    screenFlashColor_ = GREEN;
                    screenFlashAlpha_ = 0.3f;
                }
            } else if (h.kind == HazardKind::BOLT_CRATE || h.kind == HazardKind::RARITANIUM_CACHE ||
                       h.kind == HazardKind::NANOTECH_CRATE) {
                if (player_.y < 2.2f) {
                    h.active = false;
                    SpawnParticleBurst3D({ GetLaneX(h.lane), 1.0f, h.z }, 16, BROWN, 2.5f, 8.5f, 0.22f, 0.35f);
                    DamagePlayer(0.5f, true);
                }
            } else if (h.kind == HazardKind::TNT_CRATE) {
                if (player_.y < 2.1f) {
                    h.active = false;
                    DetonateTntAt(h.lane, h.z);
                    DamagePlayer(1.0f);
                }
            } else if (h.kind == HazardKind::LOW_LASER_BARRIER) {
                if (player_.y < 1.55f) {
                    h.active = false;
                    DamagePlayer(1.0f);
                }
            } else if (h.kind == HazardKind::HIGH_LASER_GATE) {
                if (player_.slideTimer <= 0.0f && player_.y > -0.1f) {
                    h.active = false;
                    DamagePlayer(1.0f);
                }
            } else if (h.kind == HazardKind::ELECTRIFIED_RAIL) {
                if (player_.y < 0.75f) {
                    DamagePlayer(1.0f);
                }
            } else if (h.kind == HazardKind::BROKEN_RAIL_GAP) {
                if (player_.y < 0.35f && player_.hyperBoostTimer <= 0.0f && player_.overdriveDuration <= 0.0f && !player_.isSwinging) {
                    DamagePlayer(1.0f);
                    player_.vy = 14.0f;
                    player_.grounded = false;
                }
            } else if (h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER) {
                if (player_.y < 2.5f) {
                    h.active = false;
                    DamagePlayer(1.0f);
                }
            } else if (h.kind == HazardKind::MAGMA_GEYSER) {
                if (player_.y < 2.6f) {
                    DamagePlayer(1.0f);
                    SpawnParticleBurst3D({ GetLaneX(h.lane), 1.5f, h.z }, 20, ORANGE, 3.5f, 10.0f, 0.30f, 0.40f);
                }
            } else if (h.kind == HazardKind::HOVER_TAXI) {
                if (player_.y < 2.4f) {
                    h.active = false;
                    DamagePlayer(1.0f);
                    SpawnParticleBurst3D({ GetLaneX(h.lane), 1.4f, h.z }, 28, YELLOW, 4.0f, 12.0f, 0.32f, 0.45f);
                    audio_.PlayExplosion();
                }
            } else if (h.kind == HazardKind::PIRANHA_LEAP) {
                float leapY = 0.5f + std::sin(h.animPhase) * 2.5f;
                if (std::abs(player_.y - leapY) < 1.3f) {
                    h.active = false;
                    DamagePlayer(1.0f);
                    SpawnParticleBurst3D({ GetLaneX(h.lane), leapY, h.z }, 22, SKYBLUE, 3.0f, 10.0f, 0.28f, 0.40f);
                    audio_.PlayExplosion();
                }
            } else if (h.kind == HazardKind::SWINGING_CRANE_SCRAP) {
                if (player_.slideTimer <= 0.0f && player_.y < 3.0f) {
                    DamagePlayer(1.0f);
                    SpawnParticleBurst3D({ GetLaneX(h.lane), 2.0f, h.z }, 20, BROWN, 3.0f, 9.0f, 0.28f, 0.40f);
                }
            }
        }

        if (h.z < -15.0f) {
            h.active = false;
        }
    }

    // Update Projectiles
    for (auto& p : projectiles_) {
        if (!p.active) continue;

        // SPECIAL 6: RYNO Infinite Homing Micro-Missiles actively steer toward nearest target in their lane!
        if (p.friendly && p.sourceWeapon == WeaponId::RYNO_INFINITE) {
            float bestZ = 999.0f;
            float targetX = GetLaneX(p.lane);
            for (const auto& h : hazards_) {
                if (!h.active || h.z <= p.pos.z || h.z > p.pos.z + 55.0f) continue;
                bool destructible = (h.kind == HazardKind::BOLT_CRATE || h.kind == HazardKind::RARITANIUM_CACHE ||
                                     h.kind == HazardKind::NANOTECH_CRATE || h.kind == HazardKind::TNT_CRATE ||
                                     h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER);
                if (destructible && h.z < bestZ) {
                    bestZ = h.z;
                    targetX = GetLaneX(h.lane);
                }
            }
            p.pos.x = Lerp(p.pos.x, targetX, 10.0f * dt);
        }

        p.pos = Vector3Add(p.pos, Vector3Scale(p.vel, dt));
        p.life -= dt;
        if (p.friendly && p.pos.z > p.maxRange) {
            SpawnParticleBurst3D(p.pos, 5, p.color, 1.0f, 3.5f, 0.14f, 0.18f);
            p.active = false;
            continue;
        }
        if (p.life <= 0.0f || p.pos.z > 135.0f || p.pos.z < -15.0f) {
            p.active = false;
            continue;
        }

        if (p.friendly) {
            // Check hit against Hazards
            for (auto& h : hazards_) {
                if (!h.active) continue;
                bool destructible = (h.kind == HazardKind::BOLT_CRATE || h.kind == HazardKind::RARITANIUM_CACHE ||
                                     h.kind == HazardKind::NANOTECH_CRATE ||
                                     h.kind == HazardKind::TNT_CRATE || h.kind == HazardKind::THUG_DRONE ||
                                     h.kind == HazardKind::SHIELDED_ENFORCER || h.kind == HazardKind::HOVER_TAXI ||
                                     h.kind == HazardKind::PIRANHA_LEAP);
                if (!destructible) continue;

                float hx = GetLaneX(h.lane);
                if (std::abs(p.pos.x - hx) < 1.65f && std::abs(p.pos.z - h.z) < 2.25f) {
                    // SPECIAL 2: Magma Combuster deals 2.5x Shield-Melter damage against Shielded Enforcers!
                    float effectiveDmg = p.damage;
                    if (p.sourceWeapon == WeaponId::COMBUSTER && h.kind == HazardKind::SHIELDED_ENFORCER) {
                        effectiveDmg *= 2.5f;
                    }
                    h.hp -= effectiveDmg;
                    SpawnParticleBurst3D(p.pos, 10, p.color, 2.0f, 8.0f, 0.22f, 0.28f);

                    // SPECIAL 5: Plasma Striker discharges EMP Chain-Lightning to shock targets on adjacent rails!
                    if (p.sourceWeapon == WeaponId::PLASMA_STRIKER) {
                        for (auto& adj : hazards_) {
                            if (!adj.active || &adj == &h) continue;
                            if (std::abs(adj.z - h.z) < 7.5f &&
                                (adj.kind == HazardKind::THUG_DRONE || adj.kind == HazardKind::SHIELDED_ENFORCER ||
                                 adj.kind == HazardKind::BOLT_CRATE || adj.kind == HazardKind::RARITANIUM_CACHE)) {
                                adj.hp -= p.damage * 0.65f;
                                SpawnParticleBurst3D({ GetLaneX(adj.lane), 1.2f, adj.z }, 12, SKYBLUE, 3.0f, 10.0f, 0.22f, 0.30f);
                                if (adj.hp <= 0.0f) {
                                    bool adjRari = (adj.kind == HazardKind::RARITANIUM_CACHE);
                                    bool adjEnemy = (adj.kind == HazardKind::THUG_DRONE || adj.kind == HazardKind::SHIELDED_ENFORCER);
                                    int adjRariDrop = adjRari
                                                    ? ((isDailyRun_ || distanceMeters_ >= 750.0f) ? 2 : 1)
                                                    : ((adjEnemy && rng_.Chance(0.20f)) ? 1 : 0);
                                    adj.active = false;
                                    SpawnBoltShower({ GetLaneX(adj.lane), 1.2f, adj.z }, 6 + adjRariDrop, adjRariDrop);
                                }
                            }
                        }
                    }

                    // SPECIAL 6: RYNO Infinite AoE Shockwave damages nearby targets across lanes!
                    if (p.sourceWeapon == WeaponId::RYNO_INFINITE) {
                        for (auto& ao : hazards_) {
                            if (!ao.active || &ao == &h) continue;
                            if (std::abs(ao.z - h.z) < 6.5f &&
                                (ao.kind == HazardKind::BOLT_CRATE || ao.kind == HazardKind::THUG_DRONE ||
                                 ao.kind == HazardKind::SHIELDED_ENFORCER)) {
                                ao.hp -= p.damage * 0.75f;
                                if (ao.hp <= 0.0f) {
                                    ao.active = false;
                                    SpawnBoltShower({ GetLaneX(ao.lane), 1.2f, ao.z }, 7, false);
                                }
                            }
                        }
                    }

                    if (h.hp <= 0.0f) {
                        bool wasTnt = (h.kind == HazardKind::TNT_CRATE);
                        bool wasEnemy = (h.kind == HazardKind::THUG_DRONE || h.kind == HazardKind::SHIELDED_ENFORCER ||
                                         h.kind == HazardKind::HOVER_TAXI || h.kind == HazardKind::PIRANHA_LEAP);
                        bool wasRari = (h.kind == HazardKind::RARITANIUM_CACHE);
                        bool wasNano = (h.kind == HazardKind::NANOTECH_CRATE);
                        int hLane = h.lane;
                        float hZ = h.z;
                        h.active = false;

                        AdvanceMultiplier();
                        AddWeaponXp(static_cast<int>(p.sourceWeapon), wasEnemy ? 18 : 8);

                        if (wasNano) {
                            player_.nanotechHp = std::min(player_.maxNanotechHp, player_.nanotechHp + 1.0f);
                            audio_.PlayLevelUp();
                        }

                        if (wasTnt) {
                            DetonateTntAt(hLane, hZ);
                            // SPECIAL 3: Constructo Shotgun safely extracts bonus Bolts when blasting TNT!
                            if (p.sourceWeapon == WeaponId::SHOCK_BLASTER) {
                                SpawnBoltShower({ hx, 1.2f, hZ }, 6, false);
                            }
                        } else {
                            audio_.PlayWrenchSmash();
                            SpawnParticleBurst3D({ hx, 1.2f, hZ }, 24, wasEnemy ? ORANGE : GOLD, 3.0f, 12.0f, 0.28f, 0.45f);
                            int boltCount = wasEnemy ? 7 : 5;
                            // SPECIAL 6: RYNO Infinite grants +50% Bonus Bolt Jackpot!
                            if (p.sourceWeapon == WeaponId::RYNO_INFINITE) {
                                boltCount = wasEnemy ? 11 : 8;
                            }
                            int shotRari = wasRari
                                         ? ((isDailyRun_ || distanceMeters_ >= 750.0f) ? 2 : 1)
                                         : ((wasEnemy && rng_.Chance(0.20f)) ? 1 : 0);
                            SpawnBoltShower({ hx, 1.2f, hZ }, boltCount + shotRari, shotRari);
                            if (wasEnemy) {
                                runKills_++;
                                save_.totalEnemiesDestroyed++;
                                ProgressContract(2, 1);
                                AdvanceRadioDare(2, 1);
                                AdvanceRadioDare(4, 1);
                                if (runKills_ >= 15) AwardSkillPoint(4); // THUG BUSTER
                            } else {
                                runCrates_++;
                                save_.totalCratesSmashed++;
                                ProgressContract(0, 1);
                                AdvanceRadioDare(4, 1);
                                GrantWeaponAmmo(IsMutatorActive(DailyMutatorId::ARMORED_CONVOY) ? 40 : 20, false);
                                if (runCrates_ >= 25) AwardSkillPoint(1); // WRENCH NINJA
                                if (wasRari) AwardSkillPoint(8);          // RARITANIUM MINER
                            }
                        }
                    }

                    // Handle Boomerang Cleave, Smart Cross-Rail Buzz Blades Ricochet, or Plasma Striker Pierce
                    if (p.bouncesLeft > 0) {
                        p.bouncesLeft--;
                        if (p.sourceWeapon == WeaponId::BUZZ_BLADES) {
                            // SPECIAL 4: Smart Cross-Rail Ricochet seeks the nearest active target across ANY of the 3 lanes!
                            int nextLane = (h.lane + 1) % NUM_LANES;
                            float bestDistZ = 999.0f;
                            for (const auto& cand : hazards_) {
                                if (!cand.active || &cand == &h || cand.z < p.pos.z - 2.0f) continue;
                                bool candDest = (cand.kind == HazardKind::BOLT_CRATE || cand.kind == HazardKind::RARITANIUM_CACHE ||
                                                 cand.kind == HazardKind::NANOTECH_CRATE || cand.kind == HazardKind::THUG_DRONE ||
                                                 cand.kind == HazardKind::SHIELDED_ENFORCER);
                                float dz = cand.z - p.pos.z;
                                if (candDest && dz >= -1.0f && dz < bestDistZ) {
                                    bestDistZ = dz;
                                    nextLane = cand.lane;
                                }
                            }
                            p.lane = nextLane;
                            p.pos.x = GetLaneX(nextLane);
                        }
                    } else {
                        p.active = false;
                    }
                    break;
                }
            }

            // Check hit against Boss Dropship Turrets
            if (p.active && boss_.active && std::abs(p.pos.z - boss_.z) < 3.5f) {
                for (int l = 0; l < NUM_LANES; ++l) {
                    if (boss_.turretHp[l] > 0.0f && std::abs(p.pos.x - GetLaneX(l)) < 1.9f) {
                        boss_.turretHp[l] = std::max(0.0f, boss_.turretHp[l] - p.damage);
                        p.active = false;
                        AddWeaponXp(static_cast<int>(p.sourceWeapon), 10);
                        SpawnParticleBurst3D(p.pos, 16, p.color, 3.0f, 10.0f, 0.28f, 0.35f);
                        if (boss_.turretHp[l] <= 0.0f) {
                            audio_.PlayExplosion();
                            cameraShake_ = 0.55f;
                            SpawnBoltShower({ GetLaneX(l), 2.5f, boss_.z - 4.0f }, 10, true);
                        }
                        break;
                    }
                }
            }
        } else {
            // Enemy Projectile vs Player (Active OmniWrench swing deflects enemy shots!)
            if (std::abs(p.pos.x - player_.x) < 1.25f && std::abs(p.pos.z) < 1.4f && player_.y < 2.0f) {
                p.active = false;
                if (player_.wrenchSwingTimer > 0.0f || player_.slideTimer > 0.0f) {
                    audio_.PlayWrenchSmash();
                    SpawnParticleBurst3D(p.pos, 12, SKYBLUE, 3.0f, 10.0f, 0.22f, 0.30f);
                } else {
                    DamagePlayer(1.0f);
                }
            }
        }
    }

    // Update Floating Bolts & Raritanium
    Vector3 playerCenter = { player_.x, player_.y + 0.9f, 0.0f };

    for (auto& b : bolts_) {
        if (!b.active) continue;

        if (b.onRail) {
            // Stationary rail-trail / jump-arc bolt moves strictly with its rail
            b.pos.z -= stepDist;

            // Clean up bolt when safely behind the camera
            if (b.pos.z < -10.0f) {
                b.active = false;
                continue;
            }

            // Only pull/collect rail bolts if Ratchet is on the SAME rail (and matching jump height)!
            // Upgrading Box Breaker Magnet in the Raritanium Lab slightly widens pickup reach on your rail.
            // Infernox Armor grants an additional +0.65m volcanic magnetic pull!
            float infBonus = (save_.equippedArmor == static_cast<int>(ArmorSuitId::INFERNOX)) ? 0.65f : 0.0f;
            float gridBonus = IsMutatorActive(DailyMutatorId::HIGH_VOLTAGE_GRID) ? 3.35f : 0.0f;
            float maxDx = 1.25f + save_.magnetLevel * 0.35f + infBonus + gridBonus;
            float maxDy = 1.35f + save_.magnetLevel * 0.25f + infBonus;
            float maxDz = 2.0f  + save_.magnetLevel * 1.1f  + infBonus * 1.5f;

            if (std::abs(b.pos.x - playerCenter.x) < maxDx &&
                std::abs(b.pos.y - playerCenter.y) < maxDy &&
                std::abs(b.pos.z - playerCenter.z) < maxDz) {
                if (b.pos.y > 1.25f || player_.y > 0.75f) {
                    AdvanceRadioDare(3, 1);
                }
                b.onRail = false; // Pull into Ratchet!
            }
        } else {
            // Scattered crate/enemy bolt shower (or a rail bolt that Ratchet just touched)
            b.pos.z -= stepDist * 0.65f;
            b.pos = Vector3Add(b.pos, Vector3Scale(b.vel, dt));
            b.vel.y -= 18.0f * dt;
            if (b.pos.y < 0.68f) {
                b.pos.y = 0.68f;
                b.vel.y = 0.0f;
                b.vel.x *= 0.85f;
            }

            Vector3 pull = Vector3Normalize(Vector3Subtract(playerCenter, b.pos));
            b.pos = Vector3Add(b.pos, Vector3Scale(pull, 56.0f * dt));
        }
        b.spin += 210.0f * dt;

        float d = Vector3Distance(b.pos, playerCenter);
        if (!b.onRail && d < 1.95f) {
            b.active = false;
            if (b.isRaritanium) {
                save_.raritanium += b.value;
                save_.totalRaritaniumEarnedLifetime += b.value;
                runRaritaniumEarned_ += b.value;
                ProgressContract(7, b.value);
                audio_.PlayRaritaniumPickup();
                AwardSkillPoint(8);
            } else {
                int gained = b.value * player_.multiplierTier;
                save_.bolts += gained;
                save_.totalBoltsEarnedLifetime += gained;
                runBoltsEarned_ += gained;
                audio_.PlayBoltPickup();
            }
        }

        if (b.pos.z < -12.0f) {
            b.active = false;
        }
    }

    if (boss_.active) {
        UpdateBossFixed(dt);
    }
}

void RailGame::UpdateBossFixed(float dt) {
    boss_.phase += 1.6f * dt;
    if (boss_.bossType == BossType::DR_NEFARIOUS_MECH) {
        // Mech actively tracks Ratchet's lateral position and hovers lower/menacingly
        boss_.hoverX = Lerp(boss_.hoverX, player_.x, 2.2f * dt);
        boss_.hoverY = 4.6f + std::sin(boss_.phase * 2.1f) * 0.45f;
    } else if (boss_.bossType == BossType::CAPTAIN_SLAG_CORSAIR) {
        // Pirate Galleon rolls smoothly side-to-side like on high seas
        boss_.hoverX = std::sin(boss_.phase * 0.85f) * 3.2f;
        boss_.hoverY = 4.4f + std::cos(boss_.phase * 1.8f) * 0.75f;
    } else {
        boss_.hoverX = std::sin(boss_.phase) * 2.2f;
        boss_.hoverY = 4.2f + std::cos(boss_.phase * 1.4f) * 0.6f;
    }

    // Check if all active turrets are destroyed
    bool allTurretsDown = true;
    for (int l = 0; l < NUM_LANES; ++l) {
        if (boss_.turretHp[l] > 0.0f) {
            allTurretsDown = false;
            break;
        }
    }
    if (allTurretsDown) {
        boss_.active = false;
        distanceSinceBoss_ = 0.0f;
        save_.totalBossesDefeated++;
        ProgressContract(8, 1);
        if (IsMutatorActive(DailyMutatorId::DEADLOCKED_GAUNTLET)) {
            player_.nanotechHp = std::min(player_.maxNanotechHp, player_.nanotechHp + 2.0f);
            SpawnBoltShower({ player_.x, player_.y + 1.5f, 4.0f }, 6, 3);
        }
        cameraShake_ = 0.9f;
        screenFlashColor_ = GOLD;
        screenFlashAlpha_ = 0.6f;
        audio_.PlayExplosion();
        audio_.PlayLevelUp();
        Color burstCol = (boss_.bossType == BossType::EMPEROR_NEFARIOUS_FRIGATE) ? PURPLE
                       : (boss_.bossType == BossType::CAPTAIN_SLAG_CORSAIR) ? SKYBLUE
                       : (boss_.bossType == BossType::DR_NEFARIOUS_MECH) ? GREEN : ORANGE;
        for (int l = 0; l < NUM_LANES; ++l) {
            SpawnParticleBurst3D({ GetLaneX(l), 3.5f, boss_.z }, 40, burstCol, 5.0f, 20.0f, 0.45f, 0.8f);
            SpawnBoltShower({ GetLaneX(l), 2.0f, 14.0f }, 12, true);
        }
        if (boss_.bossType == BossType::EMPEROR_NEFARIOUS_FRIGATE || boss_.bossType == BossType::DR_NEFARIOUS_MECH) {
            TriggerRadioTransmission(CommSpeaker::DR_NEFARIOUS, "DR. NEFARIOUS",
                                     "CURSE YOU LOMBAAAAX! This is not over! LAWRENCE, BRING ME MY ESCAPE POD!");
        } else if (boss_.bossType == BossType::CAPTAIN_SLAG_CORSAIR) {
            TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                     "Captain Slag's Sky-Galleon has been scuttled! Smooth grinding, Ratchet.");
        } else {
            TriggerRadioTransmission(CommSpeaker::CLANK, "CLANK",
                                     "Command Warship neutralized, Ratchet! Excellent shooting.");
        }
        AwardSkillPoint(6); // PROG TWIN TAKEDOWN
        return;
    }

    int minL = GetCorridorMinLaneAt(totalWorldTravel_);
    int maxL = minL + 2;

    if (boss_.bossType == BossType::EMPEROR_NEFARIOUS_FRIGATE) {
        // Sweeping Dual Imperial Laser Beams (must jump over)
        boss_.laserSweepTimer += dt;
        boss_.laserSweepX = std::sin(boss_.laserSweepTimer * 1.8f) * 5.2f;

        if (player_.y < 0.95f && std::abs(player_.x - boss_.laserSweepX) < 1.4f && player_.invulnTimer <= 0.0f && player_.hyperBoostTimer <= 0.0f) {
            DamagePlayer(1.0f);
            SpawnParticleBurst3D({ player_.x, 0.5f, 0.0f }, 16, PURPLE, 3.5f, 12.0f, 0.28f, 0.35f);
        }
    }

    // Mech Lock-On Telegraph Countdown -> Fires rapid 3-shot burst down locked lane
    if (boss_.bossType == BossType::DR_NEFARIOUS_MECH && boss_.telegraphTimer > 0.0f) {
        boss_.telegraphTimer -= dt;
        if (boss_.telegraphTimer <= 0.0f) {
            int lockedLane = std::clamp(boss_.telegraphLane, minL, maxL);
            for (int k = 0; k < 3; ++k) {
                SpawnProjectile({ GetLaneX(lockedLane), 1.3f, boss_.z - 2.0f - k * 2.8f }, { 0.0f, 0.0f, -44.0f },
                                lockedLane, WeaponId::RYNO_INFINITE, 1.0f, 0.72f, GREEN, false, 0);
            }
            audio_.PlayBlasterShot();
            boss_.telegraphLane = -1;
        }
    }

    boss_.attackCooldown -= dt;
    if (boss_.attackCooldown <= 0.0f) {
        if (boss_.bossType == BossType::PROG_TWINS) {
            // Twin Warship: Fires 2-lane synchronized plasma carpet-bombing (leaves 1 safe lane!)
            boss_.attackCooldown = 2.05f;
            int safeLane = rng_.RangeInt(minL, maxL);
            for (int l = minL; l <= maxL; ++l) {
                if (l == safeLane) continue;
                SpawnProjectile({ GetLaneX(l), 1.3f, boss_.z - 2.0f }, { 0.0f, 0.0f, -33.0f },
                                l, WeaponId::RYNO_INFINITE, 1.0f, 0.80f, ORANGE, false, 0);
            }
            audio_.PlayBlasterShot();
        } else if (boss_.bossType == BossType::EMPEROR_NEFARIOUS_FRIGATE) {
            // Frigate: Fast single-lane purple bolts alongside its sweeping ground laser
            boss_.attackCooldown = 1.65f;
            int lane = rng_.RangeInt(minL, maxL);
            SpawnProjectile({ GetLaneX(lane), 1.3f, boss_.z - 2.0f }, { 0.0f, 0.0f, -37.0f },
                            lane, WeaponId::RYNO_INFINITE, 1.0f, 0.75f, PURPLE, false, 0);
            audio_.PlayBlasterShot();
        } else if (boss_.bossType == BossType::CAPTAIN_SLAG_CORSAIR) {
            // Pirate Sky-Galleon: Staggered 3-rail broadside wave (forces lateral rail weaving!)
            boss_.attackCooldown = 2.25f;
            bool leftToRight = (static_cast<int>(boss_.phase) % 2 == 0);
            for (int step = 0; step < 3; ++step) {
                int l = leftToRight ? (minL + step) : (maxL - step);
                float zLead = boss_.z - 2.0f + step * 5.2f;
                SpawnProjectile({ GetLaneX(l), 1.25f, zLead }, { 0.0f, 0.0f, -32.0f },
                                l, WeaponId::RYNO_INFINITE, 1.0f, 0.82f, SKYBLUE, false, 0);
            }
            audio_.PlayBlasterShot();
        } else {
            // Bioliterator Dread-Mech: Locks onto Ratchet's current lane for 0.65s then unleashes 3-shot burst
            boss_.attackCooldown = 2.15f;
            boss_.telegraphLane = std::clamp(player_.currentLane, minL, maxL);
            boss_.telegraphTimer = 0.65f;
        }
    }
}

void RailGame::UpdateParticlesFixed(float dt) {
    for (auto& pt : particles_) {
        if (!pt.active) continue;
        pt.pos = Vector3Add(pt.pos, Vector3Scale(pt.vel, dt));
        pt.vel.y -= 12.0f * dt;
        pt.life -= dt;
        if (pt.life <= 0.0f) pt.active = false;
    }
}

void RailGame::RunFrame() {
    float frameDt = std::min(GetFrameTime(), 0.1f);
    accumulator_ += frameDt;
    if (screenTransitionCooldown_ > 0.0f) {
        screenTransitionCooldown_ = std::max(0.0f, screenTransitionCooldown_ - frameDt);
    }

#if !defined(PLATFORM_ANDROID)
    if (mode_ == ScreenMode::PLAYING && (!IsWindowFocused() || IsWindowMinimized())) {
        unpauseRampTimer_ = 0.0f;
        SetScreenMode(ScreenMode::PAUSED);
    }
#endif

    HandleInput();

    int musicMode = (mode_ == ScreenMode::SPLASH_INTRO || mode_ == ScreenMode::TITLE_FRONT_PAGE) ? 0 :
                    (mode_ == ScreenMode::PLAYING || mode_ == ScreenMode::PAUSED || mode_ == ScreenMode::GAME_OVER) ? 2 : 1;
    audio_.UpdateMusic(musicMode);

    while (accumulator_ >= FIXED_DT) {
        float simScale = 1.0f;
        if (mode_ == ScreenMode::PLAYING && unpauseRampTimer_ > 0.0f) {
            float frac = std::clamp(unpauseRampTimer_ / 0.75f, 0.0f, 1.0f);
            simScale = 1.0f - 0.75f * frac;
            unpauseRampTimer_ = std::max(0.0f, unpauseRampTimer_ - FIXED_DT);
        }
        UpdateFixed(FIXED_DT * simScale);
        accumulator_ -= FIXED_DT;
    }

    RenderFrame();
}

} // namespace Railfinity
