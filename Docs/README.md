# Rancage Render Engine

Rancage Render Engine adalah proyek pengembangan mesin render berbasis DirectX 12 menggunakan bahasa pemrograman C++, dilengkapi dengan editor berbasis C# WPF. Tujuan utama proyek ini adalah menciptakan mesin render yang mampu menangani model 3D kompleks seperti Sponza dengan performa tinggi dan mendukung fitur modern seperti PBR (Physically Based Rendering) dan deferred rendering. Pengembangan dilakukan secara bertahap, dengan setiap tahap didokumentasikan untuk memudahkan proses pengembangan dan kolaborasi.

## Status Saat Ini

- **Versi**: 0.0.0.9
- **Fitur**:
  - Jendela aplikasi stabil dengan resolusi 1280x720, mendukung tombol Escape untuk keluar, dan batas waktu otomatis 10 detik.
  - Rendering segitiga putih dengan rotasi dinamis 90° per detik pada sumbu Y, menggunakan latar belakang berwarna biru.
  - Penggunaan GPU NVIDIA GeForce RTX 3060 dengan VRAM 5996 MB.
  - Pipeline Direct3D 12 lengkap, meliputi command queue, allocator, list, SwapChain, RTV, DSV, Fence, RootSignature, dan PipelineState.
  - Dukungan depth buffer untuk pengujian Z-buffer.
  - Vertex buffer dan index buffer untuk segitiga (3 vertex, 3 indeks).
  - Abstraksi `IRenderable` untuk rendering modular, diimplementasikan pada kelas `Triangle`.
  - Transformasi dinamis melalui model matrix untuk mendukung animasi rotasi.
  - Pustaka matematika kustom (`Math::Matrix4`, `Math::Vector3`) untuk perhitungan matriks MVP, tanpa ketergantungan pada pustaka eksternal.
  - Sistem logging berbasis konsol, dengan output disimpan ke `Logs\engine.txt`.
- **Exit Code**: 0 (konfigurasi Debug, x64).
- **Cakupan Langkah Direct3D 12**:
  - **Inisialisasi** (15/16): Create DXGI Factory, Query Adapters, Create Device, Create Command Queue, Create Swap Chain, Create Descriptor Heaps, Create Depth Buffer, Create Fence, Serialize Root Signature, Create Pipeline State, Create Command Allocator, Create Command Lists, Create Vertex Buffer, Create Index Buffer, Enable Debug Functionality (dinonaktifkan untuk stabilitas).
  - **Setup Rendering** (3/5): Get Buffers, Create Render Target Views, Create Depth Stencil View.
  - **Rendering Loop** (8/8): Wait for Fence, Prepare Command List, Wrap up Command List, Execute Command List, Signal Fence, Queue Present, Loop kembali, Submit Draw Calls.
- **Rencana Pengembangan**:
  - **Versi 0.0.1.0**: Menambahkan dukungan tekstur (format PNG/DDS), Shader Resource Views, dan pengunggahan resource.
  - **Versi 0.1.0**: Mendukung mesh kompleks dan manajemen scene, menargetkan rendering Sponza.
  - Mengembangkan AsyncLogger untuk menangani volume log besar sebelum rendering Sponza.

## Perjalanan Pengembangan

Proyek ini mengikuti tahapan inisialisasi Direct3D 12 dengan penyesuaian untuk stabilitas dan kemudahan debugging. Berikut adalah ringkasan progres hingga Versi 0.0.0.9:

### Versi 0.0.0.1: Inisialisasi Jendela dan Debug Layer

- **Tujuan**: Membuat jendela dasar menggunakan Win32 API dan menyiapkan debug layer Direct3D 12.
- **Fitur**:
  - Jendela beresolusi 1280x720 dengan judul "Rancage Render Engine".
  - Penanganan pesan jendela (`WM_CREATE`, `WM_PAINT`, `WM_DESTROY`) melalui `WindowProc`.
  - Sistem logging sederhana untuk mencatat ke file (`Logs/engine.log`).
  - Aktivasi debug layer Direct3D 12 untuk validasi awal.
- **Kendala**:
  - Crash pada message loop jendela, menghasilkan exit code 1.
  - Logging ke file menggunakan `wofstream` sering terputus, menyebabkan crash.
  - Debug layer meningkatkan beban aplikasi dan menyebabkan crash pada beberapa konfigurasi.
- **Solusi**:
  - Menambahkan logging untuk pesan jendela (`WM_NCCREATE`, `WM_KEYDOWN`, dll.) untuk debugging.
  - Menonaktifkan output file logging, beralih ke konsol (`std::wcout`) sementara.
  - Menonaktifkan debug layer untuk meningkatkan stabilitas.

### Versi 0.0.0.2: Inisialisasi Device

- **Tujuan**: Menyiapkan device Direct3D 12 untuk komunikasi dengan GPU.
- **Fitur**:
  - Pembuatan DXGI factory untuk enumerasi adapter.
  - Pemilihan adapter dengan VRAM tertinggi (NVIDIA GeForce RTX 3060, 5996 MB).
  - Pembuatan device D3D12 dengan feature level 11.0.
  - Penambahan NVIDIA Optimus hint (`NvOptimusEnablement`) untuk memastikan pemilihan RTX 3060.
- **Kendala**:
  - Crash jendela karena proses cleanup tidak dipanggil.
  - Visual Studio mengalami crash akibat unhandled exception.
- **Solusi**:
  - Menambahkan try-catch pada `Application::Run` dan fungsi `main`.
  - Memperkuat destruktor `Window` dengan `DestroyWindow` dan `UnregisterClass`.
  - Mengarahkan output konsol ke `Logs\engine.txt` menggunakan `Engine.exe > Logs\engine.txt`.

### Versi 0.0.0.3: Command Queue, SwapChain, dan Stabilisasi

- **Tujuan**: Menyiapkan command queue dan SwapChain untuk rendering, memastikan exit code 0.
- **Fitur**:
  - Kelas `Command` untuk mengelola command queue, allocator, dan list (`D3D12_COMMAND_LIST_TYPE_DIRECT`).
  - Kelas `SwapChain` untuk double buffering (format `DXGI_FORMAT_R8G8B8A8_UNORM`, flip discard).
  - Integrasi `Command` dan `SwapChain` ke `Device` menggunakan window handle.
  - Jendela stabil dengan dukungan tombol Escape dan batas waktu 10 detik pada `Application::Run`.
  - Logging berbasis konsol dengan pengalihan ke file, mencatat informasi seperti `SwapChain created`.
  - Penambahan try-catch pada `Window::ProcessMessages` untuk menangani error `PeekMessage`/`DispatchMessage`.
- **Kendala**:
  - Upaya kembali ke file logging (`wofstream`) gagal, menyebabkan crash.
  - Logging berbasis konsol stabil namun kurang optimal untuk volume log besar.
- **Solusi**:
  - Mempertahankan logging berbasis konsol untuk stabilitas.
  - Menggunakan pengalihan ke `Logs\engine.txt` sebagai solusi sementara.
  - Merencanakan AsyncLogger untuk skalabilitas di masa depan.

### Versi 0.0.0.4: Descriptor Heaps dan Render Target Views

- **Tujuan**: Menyiapkan render target untuk output visual.
- **Fitur**:
  - Pembuatan RTV heap untuk 2 buffer (`D3D12_DESCRIPTOR_HEAP_TYPE_RTV`).
  - Pengambilan back buffer dari `SwapChain`.
  - Pembuatan Render Target Views untuk double buffering.
  - Logging berbasis konsol dengan catatan tambahan (`RTV descriptor heap created`).
- **Status**:
  - Exit code 0, siap untuk rendering dasar.

### Versi 0.0.0.5: Fence dan Clear Screen

- **Tujuan**: Menambahkan sinkronisasi GPU-CPU dan rendering warna ke jendela.
- **Fitur**:
  - Pembuatan `ID3D12Fence` untuk sinkronisasi command queue.
  - Pembersihan back buffer ke warna biru melalui RTV.
  - Presentasi SwapChain untuk menampilkan buffer.
  - Loop rendering sederhana: menunggu fence, membersihkan RTV, mengeksekusi command list, presentasi.
  - Logging berbasis konsol dengan catatan tambahan (`Fence created`, `Clearing back buffer`).
- **Kendala**:
  - Kesalahan karena kelas `Command` tidak memiliki getter `GetList`.
- **Solusi**:
  - Menambahkan `GetQueue`, `GetAllocator`, `GetList` pada `Command.h` dan `Command.cpp`.
  - Memastikan command list dibuat dan ditutup saat inisialisasi.
- **Status**:
  - Exit code 0, jendela menampilkan warna biru.

### Versi 0.0.0.6: Depth Buffer dan Depth Stencil View

- **Tujuan**: Menyiapkan Z-buffer untuk rendering geometri.
- **Fitur**:
  - Pembuatan depth/stencil texture (`DXGI_FORMAT_D32_FLOAT`).
  - Pembuatan Depth Stencil View untuk depth buffer.
  - Kelas `IResource.h` sebagai base class untuk `DepthBuffer.h`.
  - Pembaruan loop rendering untuk membersihkan DSV.
  - Logging berbasis konsol dengan catatan tambahan (`Depth buffer created`, `DSV created`).
- **Status**:
  - Exit code 0, siap untuk pipeline shader.

### Versi 0.0.0.7: Root Signature dan Pipeline State

- **Tujuan**: Menyiapkan pipeline shader untuk rendering geometri.
- **Fitur**:
  - Pembuatan root signature untuk matriks MVP.
  - Pembuatan Pipeline State Object dengan vertex/pixel shader sederhana.
  - Kelas `IPipelineComponent.h` untuk `RootSignature.h` dan `PipelineState.h`.
  - Penambahan `Vector3.h`, `Matrix4.h` untuk transformasi.
  - Rendering segitiga putih menggunakan vertex buffer sementara.
  - Logging berbasis konsol dengan catatan tambahan (`Root signature created`, `Drawing triangle`).
- **Status**:
  - Exit code 0, segitiga putih ditampilkan pada latar biru.

### Versi 0.0.0.8: Vertex Buffer, Index Buffer, dan Abstraksi Renderable

- **Tujuan**: Melengkapi pipeline geometri dengan vertex/index buffer dan struktur modular.
- **Fitur**:
  - Pembuatan vertex buffer permanen untuk 3 vertex segitiga.
  - Pembuatan index buffer untuk 3 indeks (`DXGI_FORMAT_R32_UINT`).
  - Penggantian `DrawInstanced` dengan `DrawIndexedInstanced`.
  - Kelas `IRenderable.h` sebagai interface untuk objek renderable.
  - Kelas `Triangle.h/cpp` sebagai implementasi `IRenderable` untuk segitiga.
  - Pembaruan `Application` untuk mengelola daftar `IRenderable` (saat ini 1 segitiga).
  - Logging berbasis konsol dengan catatan tambahan (`Vertex buffer created`, `Rendering triangle`).
- **Kendala**:
  - Segitiga tampak besar karena posisi kamera (z=2) dan FOV lebar (60°).
- **Solusi**:
  - Mempertahankan ukuran besar untuk visibilitas, merencanakan transformasi dinamis pada versi berikutnya.
- **Status**:
  - Exit code 0, segitiga putih besar ditampilkan pada latar biru.

### Versi 0.0.0.9: Transformasi Dinamis dan Pustaka Matematika Kustom

- **Tujuan**: Mengimplementasikan rotasi dinamis pada segitiga dan mengganti pustaka matematika eksternal dengan pustaka kustom.
- **Fitur**:
  - Penambahan transformasi dinamis menggunakan `Math::Matrix4` untuk rotasi 90° per detik pada sumbu Y.
  - Pembaruan kelas `Triangle` dengan fungsi `Update` untuk mendukung rotasi.
  - Penggantian pustaka matematika DirectX dengan `Math::Matrix4` dan `Math::Vector3` untuk perhitungan matriks MVP.
  - Pembaruan constant buffer setiap frame untuk mendukung animasi.
  - Logging berbasis konsol dengan catatan tambahan (`Updating triangle transform`, `MVP Matrix`).
- **Kendala**:
  - Pemilihan adapter awalnya bermasalah, menyebabkan crash dengan exit code 1.
  - Penggunaan pustaka matematika eksternal membuat engine kurang mandiri.
- **Solusi**:
  - Memperbaiki logika pemilihan adapter pada `Device::Initialize`.
  - Menulis ulang `Device::UpdateConstantBuffer` menggunakan `Math::Matrix4::CreateLookAt` dan `Math::CreatePerspective`.
- **Status**:
  - Exit code 0, segitiga berotasi dengan lancar, pustaka matematika sepenuhnya kustom.

## Struktur File

Berikut adalah struktur file utama dan fungsinya:

- **Core**:
  - `Application.h/cpp`: Kelas utama untuk inisialisasi `Window`, `Device`, dan menjalankan loop rendering.
  - `Window.h/cpp`: Pengelolaan jendela Win32, menangani pesan seperti `WM_PAINT` dan `WM_KEYDOWN` (Escape).
  - `Debug.h/cpp`: Pengelolaan debug layer (dinonaktifkan untuk stabilitas).
- **Graphics**:
  - `Device.h/cpp`: Mengelola `ID3D12Device`, pemilihan GPU RTX 3060, dan pembaruan constant buffer.
  - `Command.h/cpp`: Mengelola command queue, allocator, dan command list.
  - `SwapChain.h/cpp`: Mengelola double buffering dengan format `DXGI_FORMAT_R8G8B8A8_UNORM`.
  - `RootSignature.h/cpp`: Mengelola root signature untuk matriks MVP.
  - `PipelineState.h/cpp`: Mengelola Pipeline State Object untuk shader.
  - `IRenderable.h`: Interface untuk objek renderable dengan fungsi `Render` dan `Update`.
  - `Triangle.h/cpp`: Implementasi segitiga dengan vertex/index buffer dan rotasi dinamis.
- **Resources**:
  - `DepthBuffer.h/cpp`: Mengelola depth/stencil buffer dengan format `DXGI_FORMAT_D32_FLOAT`.
- **Interfaces**:
  - `IResource.h`: Kelas dasar untuk resource seperti depth buffer.
  - `IPipelineComponent.h`: Kelas dasar untuk komponen pipeline seperti root signature dan pipeline state.
- **Math**:
  - `Vector3.h/cpp`: Struktur vektor 3D untuk posisi vertex dan perhitungan kamera.
  - `Matrix4.h/cpp`: Struktur matriks 4x4 untuk transformasi matriks MVP.
- **Assets**:
  - `shaders/VertexShader.hlsl`: Shader vertex untuk transformasi menggunakan matriks MVP.
  - `shaders/PixelShader.hlsl`: Shader pixel untuk menghasilkan warna putih.
- **Utils**:
  - `Logger.h/cpp`: Sistem logging berbasis konsol dengan output ke `Logs\engine.txt`.

## Cara Menjalankan

1. **Build**:

   - Buka solusi di Visual Studio (konfigurasi Debug, platform x64).

   - Jalankan build dengan perintah:

     ```bash
     Ctrl+Shift+B
     ```

2. **Eksekusi**:

   - Jalankan aplikasi tanpa debugging:

     ```bash
     Ctrl+F5
     ```

   - Untuk menyimpan log, gunakan Command Prompt:

     ```bash
     cd D:\Projects\Visual Studio\Rancage Render Engine\x64\Debug
     Engine.exe > Logs\engine.txt
     ```

3. **Verifikasi**:

   - Periksa file `Logs\engine.txt` untuk detail runtime dan debugging.

**Hasil Eksekusi**:

- Jendela beresolusi 1280x720 menampilkan segitiga putih yang berotasi 90° per detik dengan latar belakang biru.
- Aplikasi dapat ditutup dengan menekan tombol **Escape** atau menunggu batas waktu 10 detik.

**Contoh Output Log**:

```
[INFO] Window created: Rancage Render Engine (1280x720)
[INFO] Selected adapter: NVIDIA GeForce RTX 3060 Laptop GPU
[INFO] D3D12 device created
[INFO] Triangle initialized
[INFO] Updating triangle transform
[INFO] MVP Matrix: [-0.97, 0.00, 0.04, 0.00]
[INFO] Rendering triangle
[INFO] Application exited normally
```

## Catatan

- Sistem logging berbasis konsol saat ini stabil dan memadai untuk tahap pengembangan awal.
- AsyncLogger direncanakan untuk mendukung volume log besar saat merender scene kompleks seperti Sponza.
- Kode disusun secara modular pada direktori `Core/include` dan `Core/src` untuk memudahkan pengembangan.
- Fokus pengembangan berikutnya adalah penambahan dukungan tekstur pada Versi 0.0.1.0, diikuti dengan mesh kompleks menuju rendering Sponza.

## Penutup

Proyek ini dikembangkan untuk mempelajari dan mengimplementasikan teknik rendering real-time. Inspirasi berasal dari demo rendering klasik dan scene Sponza. Saran dan masukan untuk pengembangan lebih lanjut sangat dihargai.