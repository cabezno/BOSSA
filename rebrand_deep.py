import os

def rebrand(directory):
    replacements = {
        'Shotcut': 'Bossa',
        'shotcut': 'bossa',
        'Meltytech': 'Bossa Project',
        'shotcut.org': 'bossa.io',
        'www.shotcut.org': 'bossa.io'
    }
    
    # MLT properties that MUST stay for engine compatibility
    exceptions = [
        'shotcut:rect', 'shotcut:filter', 'shotcut:hash', 'shotcut:markers',
        'shotcut:scale', 'shotcut:animIn', 'shotcut:animOut'
    ]

    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(('.cpp', '.h', '.ui', '.qml', '.js', '.json', '.txt', '.qrc', '.ts')):
                path = os.path.join(root, file)
                with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                
                original_content = content
                for old, new in replacements.items():
                    if old == 'shotcut':
                        # Special handling for shotcut: properties
                        parts = content.split('shotcut:')
                        new_content = parts[0].replace(old, new)
                        for part in parts[1:]:
                            new_content += 'shotcut:' + part
                        content = new_content
                    else:
                        content = content.replace(old, new)
                
                if content != original_content:
                    with open(path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"Rebranded: {path}")

if __name__ == "__main__":
    rebrand('src')
    rebrand('translations')
    print("Rebranding complete.")
