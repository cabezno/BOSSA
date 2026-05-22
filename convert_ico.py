from PIL import Image
import os

def convert_to_ico(png_path, ico_path):
    img = Image.open(png_path)
    img.save(ico_path, format='ICO', sizes=[(64, 64), (48, 48), (32, 32), (16, 16)])
    print(f"Converted {png_path} to {ico_path}")

if __name__ == "__main__":
    convert_to_ico("icons/bossa-logo-64.png", "packaging/windows/bossa-logo-64.ico")
