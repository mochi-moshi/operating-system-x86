from PIL import Image
import sys

def main():
    img = Image.open("../src/binaries/"+sys.argv[1], 'r')
    img = img.resize((48, 48), Image.Resampling.HAMMING)
    binary = bytearray()
    binary.extend(int.to_bytes(img.width, 4, 'little'))
    binary.extend(int.to_bytes(img.height, 4, 'little'))
    i = 0
    b = 0
    for y in range(img.height):
        for x in range(img.width):
            pix = img.getpixel((x,y))
            # binary.extend(int.to_bytes(pix[2], 1, 'little'))
            # binary.extend(int.to_bytes(pix[1], 1, 'little'))
            # binary.extend(int.to_bytes(pix[0], 1, 'little'))
            # binary.extend(int.to_bytes(pix[3], 1, 'little'))
            if pix[3] == 255:
                b |= 1 << i
            i += 1
            if i == 8:
                i = 0
                binary.append(b)
                b = 0
    name = sys.argv[1].split('.')[0]
    with open("../bin/"+name+".bin", 'wb') as f:
        f.write(binary)
if __name__ == '__main__':
    main()