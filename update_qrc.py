import os

def update_qrc(qrc_path):
    with open(qrc_path, 'r') as f:
        lines = f.readlines()
    
    end_tag_index = -1
    for i, line in enumerate(lines):
        if '</qresource>' in line:
            end_tag_index = i
            break
            
    if end_tag_index == -1:
        print("Error: </qresource> tag not found")
        return

    qml_files = []
    for root, dirs, files in os.walk('src/qml'):
        for file in files:
            if file.endswith(('.qml', 'qmldir', '.js')):
                rel_path = os.path.join(root, file).replace('src/', '').replace('\\', '/')
                qml_files.append(rel_path)
    
    existing_content = "".join(lines)
    new_entries = []
    for f in qml_files:
        if f not in existing_content:
            new_entries.append(f"        <file>{f}</file>\n")
            
    if new_entries:
        lines[end_tag_index:end_tag_index] = new_entries
        with open(qrc_path, 'w') as f:
            f.writelines(lines)
        print(f"Added {len(new_entries)} files to {qrc_path}")
    else:
        print("No new files to add to QRC.")

if __name__ == "__main__":
    update_qrc('src/resources.qrc')
