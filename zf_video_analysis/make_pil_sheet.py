from pathlib import Path
from PIL import Image, ImageDraw
import sys

src = Path(sys.argv[1])
out = Path(sys.argv[2])
files = sorted(src.glob("t*.png"))
thumbs = []
for path in files:
    image = Image.open(path).convert("RGB")
    w, h = image.size
    image = image.crop((int(w * 0.012), int(h * 0.165), int(w * 0.488), int(h * 0.805)))
    image.thumbnail((430, 260))
    canvas = Image.new("RGB", (450, 295), "white")
    canvas.paste(image, ((450 - image.width) // 2, 25))
    ms = int(path.stem[1:])
    ImageDraw.Draw(canvas).text((8, 5), f"{ms / 1000:.2f}s", fill="red")
    thumbs.append(canvas)

cols = 4
rows = (len(thumbs) + cols - 1) // cols
sheet = Image.new("RGB", (cols * 450, rows * 295), "white")
for i, thumb in enumerate(thumbs):
    sheet.paste(thumb, ((i % cols) * 450, (i // cols) * 295))
out.parent.mkdir(parents=True, exist_ok=True)
sheet.save(out)
