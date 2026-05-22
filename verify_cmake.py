import os

src_dir = 'src'
cmake_path = os.path.join(src_dir, 'CMakeLists.txt')

with open(cmake_path, 'r') as f:
    lines = f.readlines()

in_other_files = False
files_to_check = []
for line in lines:
    if 'add_custom_target(OTHER_FILES' in line:
        in_other_files = True
        continue
    if in_other_files:
        if ')' in line:
            in_other_files = False
            break
        file_path = line.strip().replace('SOURCES', '').strip()
        if file_path:
            files_to_check.append(file_path)

print(f"Checking {len(files_to_check)} files from OTHER_FILES...")
missing = []
for f in files_to_check:
    full_path = os.path.normpath(os.path.join(src_dir, f))
    if not os.path.exists(full_path):
        missing.append((f, full_path))
    else:
        print(f"OK: {f}")

if missing:
    print("\nCRITICAL: Missing files found:")
    for f, p in missing:
        print(f"  - Reference: {f}")
        print(f"    Full path: {p}")
else:
    print("\nAll files in OTHER_FILES exist.")

# Also check target_link_libraries for bossa
print("\nChecking target_link_libraries for bossa...")
# (Manual check of what I saw in previous read_file)
# PkgConfig::mlt++ -> I fixed this with IMPORTED_TARGET
# Qt6::QuickControls2 -> I added this to find_package
