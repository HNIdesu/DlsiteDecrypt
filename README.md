# DlsiteDecrypt

## Introduction
This program is designed to decrypt `.dlst` files and convert them into regular files. It is based on the [llc1123/dlsite-key-dumper](https://github.com/llc1123/dlsite-key-dumper), but the original repository is no longer maintained. The program works by dumping the decryption key from the process memory and using it to decrypt `.dlst` files.

## Usage
1. Run the script with the following command:
```
dlsitedecrypt <dlst_file_path> <dlsite_viewer_path>
```
2. Open the .dlst file you want to decrypt within the launched program.
3. Wait for the decryption and export process to complete.

## Dependencies
To run the project, you'll need to install the following dependencies:

- `pycryptodome` - A library for cryptographic operations.
- `psutil` - A library for retrieving information on running processes and system utilization.
- `kaitaistruct` - A library for working with Kaitai Struct files.

## TODO
- [x] Adapt to support [DLsite Viewer v1.1.9.0](https://mega.nz/file/YYBFCKhJ#HcG05K_s_Ujd4aNY3UpX5_17rxbNLehrAnOOrgUER7A)

## Credits
- [llc1123/dlsite-key-dumper](https://github.com/llc1123/dlsite-key-dumper) for the original implementation

## Related Projects
| Project | Comment |
| --- | --- |
| [DLsite-Play-Downloader](https://github.com/cpuopt/DLsite-Play-Downloader) | Downloads images from the DLsite player and decrypts them, involves image processing. |
| [DLSiteDumper](https://github.com/typcn/DLSiteDumper) | Dumps images by hooking certain functions. |
