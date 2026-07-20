# MinecraftPE
> [!Warning]
> [Github repository](https://github.com/Minecraft-PE-0-6-1/minecraft-pe-0.6.1-on-all) **isnt main**. All issues and pull requests should be send in [Gitea Repository](https://gitea.sffempire.ru/Kolyah35/minecraft-pe-0.6.1).

> [!Important]
> We have a discord server, where you can report bugs or send feedback https://discord.gg/c58YesBxve

Source code for **Minecraft Pocket Edition 0.6.1 alpha** with various fixes and improvements.

This project aims to preserve and improve this early version of Minecraft PE.

# Features
- Support for modern IOS and Android, Linux, Web
- Improved Dedicated Server (Windows build, anticheat, commands, etc)
- Features from Beta version of Minecraft (Grass tint, Beta sky, Smooth lighting, etc) and other tweaks

# Screenshots
<p align="center">

<img width="49%" alt="menu" src="https://sffempire.ru/mcpe-screenshots/menu.png" />
<img width="49%" alt="settings" src="https://sffempire.ru/mcpe-screenshots/settings.png" />
<img width="49%" alt="worlds" src="https://sffempire.ru/mcpe-screenshots/worlds.png" />
<img width="49%" alt="gameplay" src="https://sffempire.ru/mcpe-screenshots/gameplay.png" />

</p>

# Build

## CMake
### Linux 
1. Install dependiences

(Debian-like)

``sudo apt install build-essential git cmake libgl-dev libwayland-dev xorg-dev libxkbcommon-dev``

(Arch-like)

``sudo pacman -S base-devel git cmake libglvnd wayland xorg-server-devel xorgproto libxkbcommon``

2. Create build folder
``mkdir build && cd build``

3. Generate CMake cache and build the project
```
cmake .. -B .
cmake --build .
```

### Windows
1. Install [Visual studio Build Tools](https://aka.ms/vs/stable/vs_BuildTools.exe) and [CMake](https://github.com/Kitware/CMake/releases/download/v4.3.0-rc3/cmake-4.3.0-rc3-windows-x86_64.msi)

2. Create build folder
``mkdir build && cd build``

3. Generate CMake cache and build the project
```
cmake ..
cmake --build .
```

## Visual Studio

1. Open the repository folder in **Visual Studio**.  
2. Visual Studio will automatically detect the `CMakeLists.txt` file and generate the project configuration.  
3. Set **MinecraftPE.exe** as the **target**.  
4. Press **Run** (or F5) to build and launch the game.

## Android
### Windows
1. Download **Android NDK r14b**:  
   http://dl.google.com/android/repository/android-ndk-r14b-windows-x86_64.zip

2. Extract it to the root of your `C:` drive so the path becomes:

   ```
   C:\android-ndk-r14b
   ```
3. Install **Build tools 35** with [SDKManager](https://dl.google.com/android/repository/commandlinetools-win-15859902_latest.zip) (build-tools should be in AppData):

```powershell
sdkmanager "build-tools;35.0.0" --sdk_root="%LocalAppData%\Android\Sdk"
sdkmanager "build-tools;android-36" --sdk_root="%LocalAppData%\Android\Sdk"
```

4. Run the build script:

```powershell
# Full build (NDK + Java + APK + install)
.\build.ps1

# Skip C++ compilation (Java/assets changed only)
.\build.ps1 -NoCpp

# Skip Java compilation (C++ changed only)
.\build.ps1 -NoJava

# Only repackage + install (no compilation)
.\build.ps1 -NoBuild
```

### Linux
1. Download **Command line tools**:  
   https://developer.android.com/studio#command-line-tools-only

2. Unzip it into a folder, e.g.:

   ```bash
   mkdir -p "$HOME/Android/Sdk/"
   unzip commandlinetools-linux-*.zip -d "$HOME/Android/Sdk/"
   ```

3. Your structure should look like

   ```bash
   $HOME/Android/Sdk/cmdline-tools/bin/sdkmanager
   ```

   > [!Note] 
   > `sdkmanager` expects the SDK to include a `cmdline-tools/latest/` folder.
   > If you only have `cmdline-tools/bin`, create the required layout:
   >
   > ```bash
   > mkdir -p "$HOME/Android/Sdk/cmdline-tools/latest"
   > ln -snf ../bin  "$HOME/Android/Sdk/cmdline-tools/latest/bin"
   > ln -snf ../lib  "$HOME/Android/Sdk/cmdline-tools/latest/lib"
   > ln -snf ../source.properties "$HOME/Android/Sdk/cmdline-tools/latest/source.properties"
   > ln -snf ../NOTICE.txt        "$HOME/Android/Sdk/cmdline-tools/latest/NOTICE.txt"
   > ```

4. Install the build tools (and platform) using `sdkmanager`
   
   ```bash
   export ANDROID_SDK_ROOT="$HOME/Android/Sdk"
   export PATH="$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$PATH"

   sdkmanager --install "platform-tools" "platforms;android-35" "build-tools;35.0.0"
   ```

   > [!Note]
   > if you want build.sh to always find the SDK,
   > Set ANDROID_SDK_ROOT in your shell config (~/.bashrc / ~/.profile / ~/.config/fish/config.fish), for example:
   >
   > ```bash
   > export ANDROID_SDK_ROOT="$HOME/Android/Sdk"
   > ```
   >
   > Then restart your shell (or `source` the file)

5. Verify the install

   ```bash
   ls "$ANDROID_SDK_ROOT/build-tools"
   ```

   You should see a version folder like:

   ```bash
   35.0.0
   ```

6. Download **Android NDK r14b**:  
   https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip

7. Extract the archive to `/home/username/`, so that the final directory path is `/home/username/android-ndk-r14b/` 
   
   > [!Warning]
   > Make sure you don’t end up with a nested folder like `/home/username/android-ndk-r14b/android-ndk-r14b/`.

8. Re run `build.sh`

## Web
1. Download and install **emsdk**: https://emscripten.org/docs/getting_started/downloads.html
   > [!Note]
   > On arch linux you can use AUR:
   > `yay -Sy emsdk`
   
2. Configure and build project:
   ```
   mkdir build && cd build
   cmake .. -B . -G Ninja "-DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
   cmake --build . --target MinecraftPE
   ```
   > [!Note]
   > If you are using VSCode with CMake plugin, you can add Emscripten kit
   > 1. Press Ctrl + Shift + P
   > 2. Type `CMake: Edit User-Local CMake Kits` and hit Enter
   > 3. Add this:
      ```json
      {
         "name": "Emscripten",
         "compilers": {
            "C": "/usr/lib/emsdk/upstream/bin/clang",
            "CXX": "/usr/lib/emsdk/upstream/bin/clang++"
         },
         "toolchainFile": "/usr/lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
      }
      ```
3. Run game:
   ```
   emrun --port 8080 .
   ```
## iOS
### Xcode
> [!Note]
> There's a precompiled IPA artifact in the GitHub mirror under Actions for those who either don't have Macs or don't want to build themself. But if you want to build youself, you'll need a Mac with Xcode. Download Xcode from the Mac App Store.

### 1. Clone
Open your terminal and clone the repository
```bash
git clone https://gitea.sffempire.ru/Kolyah35/minecraft-pe-0.6.1.git
cd minecraft-pe-0.6.1
```
You can also build from the ios-support branch by checking out to it
```
git checkout ios-support
```

### 2. Open in Xcode

The project file is in `minecraft-pe-0.6.1/project/iosproj/minecraftpe.xcodeproj`. Open it.

### 3. Configure Code Signing

Before you can deploy to an iPhone, you must sign the app with your own Apple Developer account:

1. Select the **minecraftpe** project in the left sidebar.
2. Go to **Signing & Capabilities**.
3. Change the **Bundle Identifier** to something unique (e.g., `com.yourname.mcpe`).
4. Select your **Team** from the dropdown menu.

### 4. Build and Run

1. Connect your iPhone via USB or LAN.
2. Select your device from the run destination menu at the top of Xcode.
3. Press **Cmd + R** (or the Play button).
4. **Note:** If you encounter a "Developer Mode" or "Untrusted Developer" error on your phone, go to **Settings > General > VPN & Device Management** to trust your certificate.
