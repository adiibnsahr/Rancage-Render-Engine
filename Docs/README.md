# Rancage Render Engine

Rancage Render Engine adalah proyek render engine berbasis DirectX 12 (C++) dengan editor berbasis C# WPF. Tujuan utama adalah membangun engine yang mampu merender model 3D kompleks seperti Sponza dengan performa tinggi dan fitur modern (PBR, deferred rendering, dll.). Proyek ini dikembangkan secara bertahap, dengan setiap langkah didokumentasikan untuk memudahkan pengembangan dan kolaborasi.

## Status Saat Ini

- **Versi**: 0.0.0.8
- **Fitur**:
  - Window stabil (Escape, timeout 10 detik), render segitiga putih (ukuran besar) dengan latar biru.
  - Device pake RTX 3060 (5996 MB VRAM).
  - Command queue, allocator, list, SwapChain, RTV, DSV, Fence, RootSignature, PipelineState siap.
  - Depth buffer aktif untuk Z-buffer.
  - Vertex buffer dan index buffer untuk segitiga (3 vertex, 3 indeks).
  - Abstraksi `IRenderable` untuk rendering modular, implementasi awal di `Triangle`.
  - Logger console-only, redirect ke `engine.txt`.
- **Exit Code**: 0 (Debug configuration).
- **Direct3D 12 Steps Tercover**:
  - **Inisialisasi** (15/16): Enable Debug Functionality (dimatikan), Create DXGI Factory, Query Adapters, Create Device, Create Command Queue, Create Swap Chain, Create Descriptor Heaps, Create Depth Buffer, Create Fence, Serialize Root Signature, Create Pipeline State, Create Command Allocator, Create Command Lists, Create Vertex Buffer, Create Index Buffer.
  - **Setup Rendering** (3/5): Get Buffers, Create Render Target Views, Create Depth Stencil View.
  - **Rendering Loop** (8/8): Wait for Fence, Prepare Command List, Wrap up Command List, Execute Command List, Signal Fence, Queue Present, Loop kembali, Submit Draw Calls.
- **Rencana**:
  - **Versi 0.0.0.9**: Transformasi dinamis (rotasi, translasi) via model matrix, tambah `IAssetLoader.h`.
  - **Versi 0.0.1.0**: Create Textures, Create Shader Resource Views, Upload Resources.
  - AsyncLogger sebelum Sponza untuk high volume log.

## Perjalanan Pengembangan

Proyek mengikuti langkah inisialisasi Direct3D 12, dengan penyesuaian untuk stabilitas dan kemudahan debug. Berikut progres hingga versi 0.0.0.7:

### Versi 0.0.0.1: Inisialisasi Window dan Debug Layer

- **Tujuan**: Membuat window dasar menggunakan Win32 API dan menyiapkan debug layer Direct3D 12.
- **Fitur**:
  - Window dibuat dengan resolusi 1280x720, judul "Rancage Render Engine".
  - Menangani pesan dasar (`WM_CREATE`, `WM_PAINT`, `WM_DESTROY`) melalui `WindowProc`.
  - Inisialisasi logger sederhana untuk mencatat ke file (`Logs/engine.log`).
  - Setup debug layer untuk validasi Direct3D 12 (awalnya aktif).
- **Masalah**:
  - Crash di message loop window, exit code 1.
  - Logger file (`wofstream`) sering putus di `[INFO]`, menyebabkan crash.
  - Debug layer bikin aplikasi berat dan kadang crash di beberapa konfigurasi.
- **Solusi**:
  - Tambah logging pesan window (`WM_NCCREATE`, `WM_KEYDOWN`, dll.) untuk debug.
  - Nonaktifkan file output logger, ganti ke console-only (`std::wcout`) sementara.
  - Matikan debug layer untuk stabilitas (`DX12 debug layer disabled`).

### Versi 0.0.0.2: Inisialisasi Device

- **Tujuan**: Menambahkan Direct3D 12 device untuk komunikasi dengan GPU.
- **Fitur**:
  - Membuat DXGI factory untuk enumerasi adapter.
  - Memilih adapter dengan VRAM tertinggi (NVIDIA GeForce RTX 3060, 5996 MB).
  - Membuat D3D12 device dengan feature level 11.0.
  - Tambah NVIDIA Optimus hint (`NvOptimusEnablement`) untuk pastikan RTX 3060 terpilih.
- **Masalah**:
  - Crash window masih terjadi karena cleanup tidak terpanggil.
  - Visual Studio kadang ikut crash karena unhandled exception.
- **Solusi**:
  - Tambah try-catch di `Application::Run` dan `main` untuk nangkap exception.
  - Perkuat `Window::~Window` dengan `DestroyWindow` dan `UnregisterClass`.
  - Redirect console ke `Logs\engine.txt` via `Engine.exe > Logs\engine.txt` untuk debug.

### Versi 0.0.0.3: Command Queue, SwapChain, dan Stabilisasi

- **Tujuan**: Menyiapkan command queue dan SwapChain untuk rendering, sambil pastikan exit code 0.
- **Fitur**:
  - Tambah `Command` class untuk command queue, allocator, dan list (`D3D12_COMMAND_LIST_TYPE_DIRECT`).
  - Tambah `SwapChain` class untuk double buffering (format `DXGI_FORMAT_R8G8B8A8_UNORM`, flip discard).
  - Integrasi `Command` dan `SwapChain` ke `Device`, menggunakan window handle.
  - Window stabil: support Escape key, timeout 10 detik di `Application::Run`.
  - Logger console-only dengan redirect ke file, log lengkap (`SwapChain created`, `Application stopped`, `Window destroyed`).
  - Tambah try-catch di `Window::ProcessMessages` untuk handle `PeekMessage`/`DispatchMessage` error.
- **Masalah**:
  - Upaya balik ke file logger (`wofstream`) gagal, crash lagi (exit code 1).
  - Console-only logger stabil tapi kurang ideal untuk high volume log ke depan.
- **Solusi**:
  - Tetap gunakan console-only logger untuk stabilitas.
  - Redirect console output ke `Logs\engine.txt` sebagai workaround.
  - Rencanakan AsyncLogger untuk skalabilitas di masa depan.

### Versi 0.0.0.4: Descriptor Heaps dan Render Target Views

- **Tujuan**: Siapkan render target untuk output visual.
- **Fitur** (Direct3D 12 Steps):
  - **Create Descriptor Heaps**: Bikin RTV heap untuk 2 buffer (`D3D12_DESCRIPTOR_HEAP_TYPE_RTV`).
  - **Get Buffers**: Ambil back buffer dari `SwapChain`.
  - **Create Render Target Views**: Setup RTV untuk double buffering.
  - Logger console-only tetap stabil, log tambahan (`RTV descriptor heap created`, `Render target view created`).
- **Status**:
  - Exit code 0, siap untuk clear screen atau rendering sederhana.
  - Window, Device, Command, SwapChain, RTV jalan mulus.

### Versi 0.0.0.5: Fence dan Clear Screen

- **Tujuan**: Tambah sync GPU-CPU dan render warna ke window.
- **Fitur** (Direct3D 12 Steps):
  - **Create Fence**: Bikin `ID3D12Fence` untuk sync command queue.
  - **Clear Render Target**: Clear back buffer ke warna biru tua via RTV.
  - **Present SwapChain**: Tampilkan buffer di window.
  - Loop sederhana: Wait fence, clear RTV, execute command list, present.
  - Logger console-only, log tambahan (`Fence created`, `Clearing back buffer`, `Presented back buffer`).
- **Masalah**:
  - Error `class "Graphics::Command" has no member "GetList"` karena `Command` kurang getter.
- **Solusi**:
  - Tambah `GetQueue`, `GetAllocator`, `GetList` di `Command.h` dan `Command.cpp`.
  - Pastikan command list dibuat dan ditutup saat inisialisasi.
- **Status**:
  - Exit code 0, window menampilkan warna biru.
  - Siap untuk depth buffer dan pipeline.

### Versi 0.0.0.6: Depth Buffer dan Depth Stencil View

- **Tujuan**: Siapkan Z-buffer untuk rendering geometri.
- **Fitur** (Direct3D 12 Steps):
  - **Create Depth Buffer**: Bikin depth/stencil texture (`DXGI_FORMAT_D32_FLOAT`).
  - **Create Depth Stencil View**: Setup DSV untuk depth buffer.
  - Tambah `IResource.h` sebagai base class untuk `DepthBuffer.h`.
  - Update rendering loop untuk clear DSV.
  - Logger console-only, log tambahan (`Depth buffer created`, `DSV created`, `Clearing depth buffer`).
- **Status**:
  - Exit code 0, window tetap menampilkan warna biru.
  - Siap untuk root signature dan pipeline.

### Versi 0.0.0.7: Root Signature dan Pipeline State

- **Tujuan**: Siapkan shader pipeline untuk render geometri.
- **Fitur** (Direct3D 12 Steps):
  - **Serialize Root Signature**: Bikin root signature untuk MVP matrix.
  - **Create Pipeline State**: Bikin PSO dengan vertex/pixel shader sederhana.
  - Tambah `IPipelineComponent.h` untuk `RootSignature.h`, `PipelineState.h`.
  - Tambah `Vector3.h`, `Matrix4.h`, `MathUtils.h` untuk transformasi.
  - Render segitiga putih menggunakan vertex buffer sementara.
  - Logger console-only, log tambahan (`Root signature created`, `Pipeline state created`, `Drawing triangle`).
- **Status**:
  - Exit code 0, window menampilkan segitiga putih dengan latar biru.
  - Siap untuk vertex buffer dan index buffer.

### Versi 0.0.0.8: Vertex Buffer, Index Buffer, dan Abstraksi Renderable

- **Tujuan**: Lengkapi pipeline geometri dengan vertex/index buffer dan struktur modular.
- **Fitur** (Direct3D 12 Steps):
  - **Create Vertex Buffer**: Buffer permanen untuk 3 vertex segitiga.
  - **Create Index Buffer**: Buffer untuk 3 indeks (`DXGI_FORMAT_R32_UINT`).
  - **Submit Draw Calls**: Ganti `DrawInstanced` ke `DrawIndexedInstanced`.
  - Tambah `IRenderable.h` sebagai interface untuk rendering objek.
  - Tambah `Triangle.h/cpp` sebagai implementasi `IRenderable` untuk segitiga.
  - Update `Application` untuk kelola list `IRenderable` (saat ini 1 segitiga).
  - Logger console-only, log tambahan (`Vertex buffer created`, `Index buffer created`, `Rendering triangle`).
- **Masalah**:
  - Segitiga besar karena kamera dekat (z=2) dan FOV lebar (60°).
- **Solusi**:
  - Pertahankan ukuran besar untuk visibilitas, rencanakan transformasi dinamis di versi berikutnya.
- **Status**:
  - Exit code 0, window menampilkan segitiga putih besar dengan latar biru.
  - Pipeline geometri dasar hampir selesai, siap untuk transformasi dinamis.

## Struktur File

Berikut file utama dan fungsinya:

### Core

- **include/Core/Application.h, src/Core/Application.cpp**:
  - Kelas utama, inisialisasi `Window`, `Device`, loop `Run`.
  - Fitur: Timeout 10 detik, try-catch, NVIDIA Optimus hint, clear RTV/DSV, draw triangle, present.
- **include/Core/Window.h, src/Core/Window.cpp**:
  - Window Win32 (1280x720, "Rancage Render Engine").
  - Handle `WM_PAINT`, `WM_KEYDOWN` (Escape), `WM_DESTROY`.
  - Try-catch di `ProcessMessages`, log pesan (`0x%04X`).
- **include/Core/Debug.h, src/Core/Debug.cpp**:
  - Debug layer Direct3D 12 (dimatikan untuk stabilitas).

### Graphics

- **include/Graphics/Device.h, src/Graphics/Device.cpp**:
  - Kelola `ID3D12Device`, DXGI factory, pilih RTX 3060.
  - Inisialisasi `Command`, `SwapChain`, RTV/DSV heap, RTV, DSV, Fence, RootSignature, PipelineState.
  - Update constant buffer untuk MVP matrix.
- **include/Graphics/Command.h, src/Graphics/Command.cpp**:
  - Kelola command queue, allocator, list (`D3D12_COMMAND_LIST_TYPE_DIRECT`).
  - Getter: `GetQueue`, `GetAllocator`, `GetList`.
- **include/Graphics/SwapChain.h, src/Graphics/SwapChain.cpp**:
  - Kelola SwapChain (double buffer, `DXGI_FORMAT_R8G8B8A8_UNORM`).
- **include/Graphics/RootSignature.h, src/Graphics/RootSignature.cpp**:
  - Kelola root signature untuk shader (CBV untuk MVP).
  - Implementasi `IPipelineComponent`.
- **include/Graphics/PipelineState.h, src/Graphics/PipelineState.cpp**:
  - Kelola PSO dengan vertex/pixel shader.
  - Implementasi `IPipelineComponent`.
- **include/Graphics/IRenderable.h**:
  - Interface untuk objek renderable (fungsi `Render` dan `Update`).
- **include/Graphics/Triangle.h, src/Graphics/Triangle.cpp**:
  - Kelas segitiga, implementasi `IRenderable`.
  - Kelola vertex buffer (3 vertex) dan index buffer (3 indeks).

### Resources

- **include/Resources/DepthBuffer.h, src/Resources/DepthBuffer.cpp**:
  - Kelola depth/stencil buffer (`DXGI_FORMAT_D32_FLOAT`).
  - Implementasi `IResource`.

### Interfaces

- **include/Interfaces/IResource.h**:
  - Base class untuk resource (`DepthBuffer`, nanti `VertexBuffer`).
- **include/Interfaces/IPipelineComponent.h**:
  - Base class untuk pipeline (`RootSignature`, `PipelineState`).

### Math

- **include/Math/Vector3.h, src/Math/Vector3.cpp**:
  - Vektor 3D untuk posisi vertex.
- **include/Math/Matrix4.h, src/Math/Matrix4.cpp**:
  - Matriks 4x4 untuk transformasi MVP.
- **include/Math/MathUtils.h, src/Math/MathUtils.cpp**:
  - Placeholder untuk helper math.

### Assets

- **assets/shaders/VertexShader.hlsl**:
  - Vertex shader untuk transformasi MVP.
- **assets/shaders/PixelShader.hlsl**:
  - Pixel shader untuk warna putih.

### Utils

- **include/Utils/Logger.h, src/Core/Logger.cpp**:
  - Console-only logger (`std::wcout`), thread-safe (mutex).
  - Level: `[INFO]`, `[WARNING]`, `[ERROR]`.
  - Redirect ke `Logs\engine.txt` via `Engine.exe > Logs\engine.txt`.
  - Note: File output crash, ditunda ke AsyncLogger.

## Cara Menjalankan

1. Build di Visual Studio (Debug, x64):

   ```bash
   Ctrl+Shift+B
   ```

2. Jalankan tanpa debug:

   ```bash
   Ctrl+F5
   ```

   Atau via Command Prompt untuk simpan log:

   ```bash
   cd D:\Projects\Visual Studio\Rancage Render Engine\x64\Debug
   Engine.exe > Logs\engine.txt
   ```

3. Cek `Logs\engine.txt` untuk debug.

## Catatan

- Console-only logger stabil, cukup untuk render awal.
- AsyncLogger direncanakan untuk skalabilitas saat render Sponza.
- File di `EngineCore/include` dan `EngineCore/src` modular.
- Fokus berikutnya: Transformasi dinamis untuk animasi segitiga, lalu tekstur untuk material.