import os

def update_qml_files(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(('.qml', 'qmldir', '.js')):
                path = os.path.join(root, file)
                try:
                    with open(path, 'r', encoding='utf-8') as f:
                        content = f.read()
                    
                    new_content = content.replace('Shotcut', 'Bossa').replace('shotcut', 'bossa')
                    
                    if new_content != content:
                        with open(path, 'w', encoding='utf-8') as f:
                            f.write(new_content)
                        print(f"Updated: {path}")
                except Exception as e:
                    print(f"Error processing {path}: {e}")

if __name__ == "__main__":
    update_qml_files('src/qml')
    print("QML Update complete.")
