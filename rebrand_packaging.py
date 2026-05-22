import os

def rebrand(directory):
    replacements = {
        'Shotcut': 'Bossa',
        'shotcut': 'bossa',
        'Meltytech': 'Bossa Project',
        'com.Meltytech.Shotcut': 'io.bossa.Bossa',
        'org.shotcut.Shotcut': 'org.bossa.Bossa',
        'shotcut.org': 'bossa.io',
        'www.shotcut.org': 'bossa.io',
        'shotcut.icns': 'bossa.icns'
    }

    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(('.xml', '.in', '.desktop', '.yaml', '.plist', '.1', '.iss')):
                path = os.path.join(root, file)
                with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                original_content = content
                for old, new in replacements.items():
                    content = content.replace(old, new)
                
                if content != original_content:
                    with open(path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"Rebranded: {path}")

if __name__ == "__main__":
    rebrand('packaging')
    print("Packaging rebranding complete.")
