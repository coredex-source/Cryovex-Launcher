# 🚀 Cryovex Minecraft Launcher

A modern Minecraft launcher built with Qt 6 and C++, featuring Microsoft authentication and a beautiful Material Design interface.

## ✨ Features

### ✅ Phase 1: Core Setup (COMPLETED)
- ✅ Qt 6 project structure with CMake
- ✅ Modern QML UI with Material Design
- ✅ Modular architecture with clean separation
- ✅ Organized folder structure for all components

### ✅ Phase 2: Microsoft Authentication (COMPLETED)
- ✅ Full Microsoft OAuth2 implementation
- ✅ Xbox Live token exchange
- ✅ XSTS authentication
- ✅ Minecraft Services integration
- ✅ Profile fetching and token management
- ✅ Persistent authentication storage

### 🔄 Upcoming Phases
- [ ] **Phase 3**: Version Management
- [ ] **Phase 4**: Download System  
- [ ] **Phase 5**: Game Launching
- [ ] **Phase 6**: Profiles & Settings
- [ ] **Phase 7**: UI Polish

## 🛠️ Prerequisites

### Required Software
1. **Qt 6.5+** with the following modules:
   - Qt Core
   - Qt Widgets
   - Qt Quick
   - Qt Quick Controls 2
   - Qt Network
   - Qt WebEngine

2. **CMake 3.16+** or **qmake**

3. **C++17 compatible compiler**:
   - MSVC 2019+ (Windows)
   - GCC 9+ (Linux)
   - Clang 9+ (macOS)

### Microsoft App Registration
Before testing authentication, you need to:

1. Go to [Azure App Registrations](https://portal.azure.com/#blade/Microsoft_AAD_IAM/ActiveDirectoryMenuBlade/RegisteredApps)
2. Create a new application registration
3. Set redirect URI to: `http://localhost:8080/auth/callback`
4. Copy your **Client ID**
5. Replace `YOUR_CLIENT_ID` in `src/auth/MicrosoftAuth.cpp`

## 🚀 Building

### Option 1: CMake (Recommended)
```bash
mkdir build
cd build
cmake ..
make  # or cmake --build .
```

### Option 2: qmake
```bash
qmake CryovexLauncher.pro
make  # or nmake on Windows
```

## 📁 Project Structure

```
Cryovex Launcher/
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── auth/                    # Microsoft authentication
│   │   ├── AuthManager.*        # Main auth coordinator
│   │   └── MicrosoftAuth.*      # OAuth2 implementation
│   ├── version/                 # Minecraft version management
│   │   ├── VersionManager.*     # Version list handling
│   │   └── MinecraftVersion.*   # Version data model
│   ├── download/                # Download system
│   │   ├── DownloadManager.*    # Concurrent downloads
│   │   └── DownloadTask.*       # Individual download
│   ├── launcher/                # Game launching
│   │   ├── GameLauncher.*       # Process management
│   │   └── JvmArgumentBuilder.* # JVM args construction
│   ├── config/                  # Settings & profiles
│   │   ├── ConfigManager.*      # Settings storage
│   │   └── Profile.*            # User profile data
│   └── utils/                   # Utilities
│       ├── Logger.*             # Logging system
│       ├── FileUtils.*          # File operations
│       └── NetworkUtils.*       # Network helpers
├── qml/                         # QML user interface
│   ├── main.qml                 # Main window
│   ├── LoginView.qml            # Authentication UI
│   ├── MainView.qml             # Launcher interface
│   └── SettingsView.qml         # Settings panel
└── CMakeLists.txt               # Build configuration
```

## 🔐 Authentication Flow

The launcher implements the complete Microsoft authentication flow:

1. **OAuth2 Authorization**: Opens web view for Microsoft login
2. **Token Exchange**: Exchanges auth code for access/refresh tokens
3. **Xbox Live Auth**: Authenticates with Xbox Live services
4. **XSTS Auth**: Gets Xbox Security Token Service token
5. **Minecraft Auth**: Exchanges XSTS token for Minecraft access token
6. **Profile Fetch**: Retrieves Minecraft profile (username, UUID)

## 🎨 UI Features

- **Modern Material Design** with dark theme
- **Windows 11 Mica-style** translucent effects
- **Responsive layout** that adapts to different screen sizes
- **Smooth animations** and transitions
- **Accessible** with proper focus handling

## 🧪 Testing

To test the authentication flow:

1. Ensure Qt 6 and dependencies are installed
2. Register your Microsoft app and update the Client ID
3. Build and run the launcher
4. Click "Sign in with Microsoft"
5. Complete the authentication in the web view
6. Verify the user profile is displayed

## 📋 Development Status

### Completed ✅
- [x] Project structure and build system
- [x] Authentication system (OAuth2 → Xbox Live → XSTS → Minecraft)
- [x] User interface framework with Material Design
- [x] Configuration management foundation
- [x] Logging and utility systems

### In Progress 🔄
- [ ] Version manifest downloading and parsing
- [ ] Download manager with progress tracking
- [ ] Game launching with proper JVM arguments

### Planned 📅
- [ ] Mod support and management
- [ ] Skin preview and customization
- [ ] Launch profiles with different settings
- [ ] Update system for the launcher itself

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## ⚠️ Disclaimer

This launcher is not affiliated with Mojang Studios or Microsoft. Minecraft is a trademark of Mojang Studios.


## Version scheme
Early Development:
- Cryovex-Proto-x.y.z → prototype builds (barebones, experimental).
- Cryovex-Fragment-x.y.z → pre alpha builds.

Alpha Stage (unstable, feature testing):
- Cryovex-Shard-x.y.z → small, fragile, but functional builds.
- Cryovex-Frostbite-x.y.z → rough but powerful test versions.

Beta Stage (feature complete, testing/bugfixes):
- Cryovex-Glacier-x.y.z → stable core forming, slow but solid.
- Cryovex-Oblivion-x.y.z → big sweeping experimental updates.

Release Candidates:
- Cryovex-Crystal-x.y.z-rcN → polished, almost ready for stable release.
- Cryovex-Prism-x.y.z-rcN → bugfixes before release.

Stable Releases:
- Cryovex-Core-x.y.z → stable “mainline” release.
- Cryovex-Aurora-x.y.z → not so stable experimental updates.
- Cryovex-Singularity-x.y.z → very stable experimental updates.