from PIL import Image, ImageDraw, ImageFont
import os

def create_logo(path, size, text):
    # Create a black background image
    img = Image.new('RGB', (size, size), color=(0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    try:
        font = ImageFont.truetype("arial.ttf", int(size/4))
    except:
        font = ImageFont.load_default()
    
    try:
        bbox = draw.textbbox((0, 0), text, font=font)
        w = bbox[2] - bbox[0]
        h = bbox[3] - bbox[1]
    except:
        w, h = draw.textsize(text, font=font)
        
    x = (size - w) / 2
    y = (size - h) / 2
    
    draw.rectangle([10, 10, size-10, size-10], outline=(0, 229, 255), width=5)
    draw.text((x, y), text, fill=(0, 229, 255), font=font)
    
    img.save(path)
    print(f"Created logo at {path}")

if __name__ == "__main__":
    create_logo("icons/bossa-logo-320x320.png", 320, "BOSSA PRO")
    create_logo("icons/bossa-logo-64.png", 64, "B")
