import os
import re

def rebrand(dir_path):
    extensions = ('.cpp', '.h', '.ui', '.qml', 'CMakeLists.txt')
    
    # Regex for Shotcut -> Bossa (case sensitive)
    shotcut_pattern = re.compile(r'Shotcut')
    
    # Regex for shotcut -> bossa (case sensitive)
    # Avoid replacing if it looks like an MLT property: shotcut:, shotcut_, _shotcut:
    # (?<!_)shotcut(?![_:])
    bossa_pattern = re.compile(r'(?<!_)shotcut(?![_:])')

    for root, dirs, files in os.walk(dir_path):
        for file in files:
            if file.endswith(extensions) or file == 'CMakeLists.txt':
                file_path = os.path.join(root, file)
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                new_content = shotcut_pattern.sub('Bossa', content)
                new_content = bossa_pattern.sub('bossa', new_content)
                
                if new_content != content:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    print(f"Rebranded: {file_path}")

if __name__ == "__main__":
    rebrand(r'C:\Users\User\bossa-editor\src')
    # Also process the root CMakeLists.txt
    rebrand(r'C:\Users\User\bossa-editor')
