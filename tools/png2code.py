#!/usr/bin/python3
# png画像からドット位置のバイナリをテキストで出力する
# TFT_eSPIのSprite::pushImage用ヘッダファイル

import numpy as np
from PIL import Image
import sys
import os

in_name = sys.argv[1]
out_name = in_name + '.h'

# RGB値を16bit(RGB:565)のテキストに変換する
def rgb2hexstr(rgb):
    col = ((rgb[0]>>3)<<11) | ((rgb[1]>>2)<<5) | (rgb[2]>>3)
    return "0x{:04X}".format(col)

with open(in_name, 'rb') as infile:
  with open(out_name, 'wb+') as outfile:
    outfile.write('#include <stdint.h>\n#include <pgmspace.h>\n\n'.encode('utf-8'))
    arrary_name = 'PROGMEM const uint16_t ' + out_name[0:out_name.find('.')] + '[] = {\n'
    outfile.write(arrary_name.encode('utf-8'))
    image = Image.open(sys.argv[1])
    width, height = image.size

    comment = '// w,h:' + str(width) + ',' + str(height) + '\n'
    outfile.write(comment.encode('utf-8'))

    #  パレット読み込み
    if image.mode == 'P':
        palette = np.array(image.getpalette()).reshape(-1, 3)
        getPixel = lambda x,y: palette[image.getpixel((x, y))]
    else:
        getPixel = lambda x,y: image.getpixel((x, y))

    x_cnt = 0
    for y in range(height):
        for x in range(width):
            pixel = getPixel(x, y)
            data = rgb2hexstr(pixel) + ","
            outfile.write(data.encode('utf-8'))
            x_cnt = x_cnt + 1
            if x_cnt >= 8:
                x_cnt = 0
                outfile.write('\n'.encode('utf-8'))
        x_cnt = 0

    outfile.write('};\n'.encode('utf-8'))

print('Out:'+ out_name +' Done!')

