/**
 * The MIT License (MIT)
 *
 * This library is written and maintained by Richard Moore.
 * Major parts were derived from Project Nayuki's library.
 *
 * Copyright (c) 2017 Richard Moore     (https://github.com/ricmoo/QRCode)
 * Copyright (c) 2017 Project Nayuki    (https://www.nayuki.io/page/qr-code-generator-library)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Special thanks to Nayuki (https://www.nayuki.io/) from which this library was
 *  heavily inspired and compared against.
 *
 *  See: https://github.com/nayuki/QR-Code-generator/tree/master/cpp
 */

// @todo: Code a optimiser d'avantage

#include "qrcode.h"
#include <stdlib.h>
#include <string.h>

static const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    26, 15, 44, 36};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    1, 1, 2, 2};

static const uint16_t NUM_RAW_DATA_MODULES = 567;

static int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

static char getModeBits(uint8_t version, uint8_t mode)
{
    unsigned int modeInfo = 0x7bbb80a;

    char result = 8 + ((modeInfo >> (3 * mode)) & 0x07);
    if (result == 15)
    {
        result = 16;
    }

    return result;
}

typedef struct BitBucket
{
    uint32_t bitOffsetOrWidth;
    uint16_t capacityBytes;
    uint8_t *data;
} BitBucket;

static uint16_t bb_getGridSizeBytes(uint8_t size)
{
    return (((size * size) + 7) / 8);
}

static uint16_t bb_getBufferSizeBytes(uint32_t bits)
{
    return ((bits + 7) / 8);
}

static void bb_initBuffer(BitBucket *bitBuffer, uint8_t *data, int32_t capacityBytes)
{
    bitBuffer->bitOffsetOrWidth = 0;
    bitBuffer->capacityBytes = capacityBytes;
    bitBuffer->data = data;

    memset(data, 0, bitBuffer->capacityBytes);
}

static void bb_initGrid(BitBucket *bitGrid, uint8_t *data, uint8_t size)
{
    bitGrid->bitOffsetOrWidth = size;
    bitGrid->capacityBytes = bb_getGridSizeBytes(size);
    bitGrid->data = data;

    memset(data, 0, bitGrid->capacityBytes);
}

static void bb_appendBits(BitBucket *bitBuffer, uint32_t val, uint8_t length)
{
    uint32_t offset = bitBuffer->bitOffsetOrWidth;
    for (int8_t i = length - 1; i >= 0; i--, offset++)
    {
        bitBuffer->data[offset >> 3] |= ((val >> i) & 1) << (7 - (offset & 7));
    }
    bitBuffer->bitOffsetOrWidth = offset;
}

static void bb_setBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool on)
{
    uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    uint8_t mask = 1 << (7 - (offset & 0x07));
    if (on)
    {
        bitGrid->data[offset >> 3] |= mask;
    }
    else
    {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static void bb_invertBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool invert)
{
    uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    uint8_t mask = 1 << (7 - (offset & 0x07));
    bool on = ((bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0);
    if (on ^ invert)
    {
        bitGrid->data[offset >> 3] |= mask;
    }
    else
    {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static bool bb_getBit(BitBucket *bitGrid, uint8_t x, uint8_t y)
{
    uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    return (bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
}

static void applyMask(BitBucket *modules, BitBucket *isFunction, uint8_t mask)
{
    uint8_t size = modules->bitOffsetOrWidth;

    for (uint8_t y = 0; y < size; y++)
    {
        for (uint8_t x = 0; x < size; x++)
        {
            if (bb_getBit(isFunction, x, y))
            {
                continue;
            }

            bool invert = 0;
            switch (mask)
            {
            case 0:
                invert = (x + y) % 2 == 0;
                break;
            case 1:
                invert = y % 2 == 0;
                break;
            case 2:
                invert = x % 3 == 0;
                break;
            case 3:
                invert = (x + y) % 3 == 0;
                break;
            case 4:
                invert = (x / 3 + y / 2) % 2 == 0;
                break;
            case 5:
                invert = x * y % 2 + x * y % 3 == 0;
                break;
            case 6:
                invert = (x * y % 2 + x * y % 3) % 2 == 0;
                break;
            case 7:
                invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
                break;
            }
            bb_invertBit(modules, x, y, invert);
        }
    }
}

static void setFunctionModule(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y, bool on)
{
    bb_setBit(modules, x, y, on);
    bb_setBit(isFunction, x, y, true);
}

// Draws a 9*9 finder pattern including the border separator, with the center module at (x, y).
static void drawFinderPattern(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y)
{
    uint8_t size = modules->bitOffsetOrWidth;

    for (int8_t i = -4; i <= 4; i++)
    {
        for (int8_t j = -4; j <= 4; j++)
        {
            uint8_t dist = max(abs(i), abs(j)); // Chebyshev/infinity norm
            int16_t xx = x + j, yy = y + i;
            if (0 <= xx && xx < size && 0 <= yy && yy < size)
            {
                setFunctionModule(modules, isFunction, xx, yy, dist != 2 && dist != 4);
            }
        }
    }
}

// Draws a 5*5 alignment pattern, with the center module at (x, y).
static void drawAlignmentPattern(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y)
{
    for (int8_t i = -2; i <= 2; i++)
    {
        for (int8_t j = -2; j <= 2; j++)
        {
            setFunctionModule(modules, isFunction, x + j, y + i, max(abs(i), abs(j)) != 1);
        }
    }
}

// Draws two copies of the format bits (with its own error correction code)
// based on the given mask and this object's error correction level field.
static void drawFormatBits(BitBucket *modules, BitBucket *isFunction, uint8_t ecc, uint8_t mask)
{
    uint8_t size = modules->bitOffsetOrWidth;

    // Calculate error correction code and pack bits
    uint32_t data = ecc << 3 | mask; // errCorrLvl is uint2, mask is uint3
    uint32_t rem = data;
    for (int i = 0; i < 10; i++)
    {
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    }

    data = data << 10 | rem;
    data ^= 0x5412; // uint15

    // Draw first copy
    for (uint8_t i = 0; i <= 5; i++)
    {
        setFunctionModule(modules, isFunction, 8, i, ((data >> i) & 1) != 0);
    }

    setFunctionModule(modules, isFunction, 8, 7, ((data >> 6) & 1) != 0);
    setFunctionModule(modules, isFunction, 8, 8, ((data >> 7) & 1) != 0);
    setFunctionModule(modules, isFunction, 7, 8, ((data >> 8) & 1) != 0);

    for (int8_t i = 9; i < 15; i++)
    {
        setFunctionModule(modules, isFunction, 14 - i, 8, ((data >> i) & 1) != 0);
    }

    // Draw second copy
    for (int8_t i = 0; i <= 7; i++)
    {
        setFunctionModule(modules, isFunction, size - 1 - i, 8, ((data >> i) & 1) != 0);
    }

    for (int8_t i = 8; i < 15; i++)
    {
        setFunctionModule(modules, isFunction, 8, size - 15 + i, ((data >> i) & 1) != 0);
    }

    setFunctionModule(modules, isFunction, 8, size - 8, true);
}

static void drawFunctionPatterns(BitBucket *modules, BitBucket *isFunction, uint8_t version, uint8_t ecc)
{
    uint8_t size = modules->bitOffsetOrWidth;

    // Draw the horizontal and vertical timing patterns
    for (uint8_t i = 0; i < size; i++)
    {
        setFunctionModule(modules, isFunction, 6, i, i % 2 == 0);
        setFunctionModule(modules, isFunction, i, 6, i % 2 == 0);
    }

    // Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
    drawFinderPattern(modules, isFunction, 3, 3);
    drawFinderPattern(modules, isFunction, size - 4, 3);
    drawFinderPattern(modules, isFunction, 3, size - 4);

    drawAlignmentPattern(modules, isFunction, 22, 22);
    drawFormatBits(modules, isFunction, ecc, 0);
}

// Draws the given sequence of 8-bit codewords (data and error correction) onto the entire
// data area of this QR Code symbol. Function modules need to be marked off before this is called.
static void drawCodewords(BitBucket *modules, BitBucket *isFunction, BitBucket *codewords)
{

    uint32_t bitLength = codewords->bitOffsetOrWidth;
    uint8_t *data = codewords->data;

    uint8_t size = modules->bitOffsetOrWidth;

    // Bit index into the data
    uint32_t i = 0;

    // Do the funny zigzag scan
    for (int16_t right = size - 1; right >= 1; right -= 2)
    { // Index of right column in each column pair
        if (right == 6)
        {
            right = 5;
        }

        for (uint8_t vert = 0; vert < size; vert++)
        { // Vertical counter
            for (int j = 0; j < 2; j++)
            {
                uint8_t x = right - j; // Actual x coordinate
                bool upwards = ((right & 2) == 0) ^ (x < 6);
                uint8_t y = upwards ? size - 1 - vert : vert; // Actual y coordinate
                if (!bb_getBit(isFunction, x, y) && i < bitLength)
                {
                    bb_setBit(modules, x, y, ((data[i >> 3] >> (7 - (i & 7))) & 1) != 0);
                    i++;
                }
                // If there are any remainder bits (0 to 7), they are already
                // set to 0/false/white when the grid of modules was initialized
            }
        }
    }
}

#define PENALTY_N1 3
#define PENALTY_N2 3
#define PENALTY_N3 40
#define PENALTY_N4 10

static uint32_t getPenaltyScore(BitBucket *modules)
{
    uint32_t result = 0;

    uint8_t size = modules->bitOffsetOrWidth;

    // Adjacent modules in row having same color
    for (uint8_t y = 0; y < size; y++)
    {

        bool colorX = bb_getBit(modules, 0, y);
        for (uint8_t x = 1, runX = 1; x < size; x++)
        {
            bool cx = bb_getBit(modules, x, y);
            if (cx != colorX)
            {
                colorX = cx;
                runX = 1;
            }
            else
            {
                runX++;
                if (runX == 5)
                {
                    result += PENALTY_N1;
                }
                else if (runX > 5)
                {
                    result++;
                }
            }
        }
    }

    // Adjacent modules in column having same color
    for (uint8_t x = 0; x < size; x++)
    {
        bool colorY = bb_getBit(modules, x, 0);
        for (uint8_t y = 1, runY = 1; y < size; y++)
        {
            bool cy = bb_getBit(modules, x, y);
            if (cy != colorY)
            {
                colorY = cy;
                runY = 1;
            }
            else
            {
                runY++;
                if (runY == 5)
                {
                    result += PENALTY_N1;
                }
                else if (runY > 5)
                {
                    result++;
                }
            }
        }
    }

    uint16_t black = 0;
    for (uint8_t y = 0; y < size; y++)
    {
        uint16_t bitsRow = 0, bitsCol = 0;
        for (uint8_t x = 0; x < size; x++)
        {
            bool color = bb_getBit(modules, x, y);

            // 2*2 blocks of modules having same color
            if (x > 0 && y > 0)
            {
                bool colorUL = bb_getBit(modules, x - 1, y - 1);
                bool colorUR = bb_getBit(modules, x, y - 1);
                bool colorL = bb_getBit(modules, x - 1, y);
                if (color == colorUL && color == colorUR && color == colorL)
                {
                    result += PENALTY_N2;
                }
            }

            // Finder-like pattern in rows and columns
            bitsRow = ((bitsRow << 1) & 0x7FF) | color;
            bitsCol = ((bitsCol << 1) & 0x7FF) | bb_getBit(modules, y, x);

            // Needs 11 bits accumulated
            if (x >= 10)
            {
                if (bitsRow == 0x05D || bitsRow == 0x5D0)
                {
                    result += PENALTY_N3;
                }
                if (bitsCol == 0x05D || bitsCol == 0x5D0)
                {
                    result += PENALTY_N3;
                }
            }

            // Balance of black and white modules
            if (color)
            {
                black++;
            }
        }
    }

    // Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
    uint16_t total = size * size;
    for (uint16_t k = 0; black * 20 < (9 - k) * total || black * 20 > (11 + k) * total; k++)
    {
        result += PENALTY_N4;
    }

    return result;
}

static uint8_t rs_multiply(uint8_t x, uint8_t y)
{
    // Russian peasant multiplication
    // See: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
    uint16_t z = 0;
    for (int8_t i = 7; i >= 0; i--)
    {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return z;
}

static void rs_init(uint8_t degree, uint8_t *coeff)
{
    memset(coeff, 0, degree);
    coeff[degree - 1] = 1;

    // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
    // drop the highest term, and store the rest of the coefficients in order of descending powers.
    // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
    uint16_t root = 1;
    for (uint8_t i = 0; i < degree; i++)
    {
        // Multiply the current product by (x - r^i)
        for (uint8_t j = 0; j < degree; j++)
        {
            coeff[j] = rs_multiply(coeff[j], root);
            if (j + 1 < degree)
            {
                coeff[j] ^= coeff[j + 1];
            }
        }
        root = (root << 1) ^ ((root >> 7) * 0x11D); // Multiply by 0x02 mod GF(2^8/0x11D)
    }
}

static void rs_getRemainder(uint8_t degree, uint8_t *coeff, uint8_t *data, uint8_t length, uint8_t *result, uint8_t stride)
{
    // Compute the remainder by performing polynomial division

    // for (uint8_t i = 0; i < degree; i++) { result[] = 0; }
    // memset(result, 0, degree);

    for (uint8_t i = 0; i < length; i++)
    {
        uint8_t factor = data[i] ^ result[0];
        for (uint8_t j = 1; j < degree; j++)
        {
            result[(j - 1) * stride] = result[j * stride];
        }
        result[(degree - 1) * stride] = 0;

        for (uint8_t j = 0; j < degree; j++)
        {
            result[j * stride] ^= rs_multiply(coeff[j], factor);
        }
    }
}

static int8_t encodeDataCodewords(BitBucket *dataCodewords, const uint8_t *text, uint16_t length, uint8_t version)
{
    bb_appendBits(dataCodewords, 1 << MODE_BYTE, 4);

    bb_appendBits(dataCodewords, length, getModeBits(version, MODE_BYTE));

    for (uint16_t i = 0; i < length; i++)
    {
        bb_appendBits(dataCodewords, (char)(text[i]), 8);
    }

    return MODE_BYTE;
}

static void performErrorCorrection(uint8_t version, uint8_t ecc, BitBucket *data)
{

    // See: http://www.thonky.com/qr-code-tutorial/structure-final-message

    uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecc];
    uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ecc];
    uint16_t moduleCount = NUM_RAW_DATA_MODULES;

    uint8_t blockEccLen = totalEcc / numBlocks;
    uint8_t numShortBlocks = numBlocks - moduleCount / 8 % numBlocks;
    uint8_t shortBlockLen = moduleCount / 8 / numBlocks;

    uint8_t shortDataBlockLen = shortBlockLen - blockEccLen;

    uint8_t result[data->capacityBytes];
    memset(result, 0, sizeof(result));

    uint8_t coeff[blockEccLen];
    rs_init(blockEccLen, coeff);

    uint16_t offset = 0;
    uint8_t *dataBytes = data->data;

    // Interleave all short blocks
    for (uint8_t i = 0; i < shortDataBlockLen; i++)
    {
        uint16_t index = i;
        uint8_t stride = shortDataBlockLen;
        for (uint8_t blockNum = 0; blockNum < numBlocks; blockNum++)
        {
            result[offset++] = dataBytes[index];

            index += stride;
        }
    }

    // Add all ecc blocks, interleaved
    uint8_t blockSize = shortDataBlockLen;
    for (uint8_t blockNum = 0; blockNum < numBlocks; blockNum++)
    {
        rs_getRemainder(blockEccLen, coeff, dataBytes, blockSize, &result[offset + blockNum], numBlocks);
        dataBytes += blockSize;
    }

    memcpy(data->data, result, data->capacityBytes);
    data->bitOffsetOrWidth = moduleCount;
}

static const uint8_t ECC_FORMAT_BITS = (0x02 << 6) | (0x03 << 4) | (0x00 << 2) | (0x01 << 0);

uint16_t qrcode_getBufferSize(uint8_t version)
{
    return bb_getGridSizeBytes(4 * version + 17);
}

int8_t qrcode_initBytes(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, uint8_t *data, uint16_t length)
{
    uint8_t size = version * 4 + 17;
    qrcode->version = version;
    qrcode->size = size;
    qrcode->ecc = ecc;
    qrcode->modules = modules;

    uint8_t eccFormatBits = (ECC_FORMAT_BITS >> (2 * ecc)) & 0x03;

    uint16_t moduleCount = NUM_RAW_DATA_MODULES;
    uint16_t dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[eccFormatBits];

    struct BitBucket codewords;
    uint8_t codewordBytes[bb_getBufferSizeBytes(moduleCount)];
    bb_initBuffer(&codewords, codewordBytes, (int32_t)sizeof(codewordBytes));

    int8_t mode = encodeDataCodewords(&codewords, data, length, version);

    if (mode < 0)
    {
        return -1;
    }
    qrcode->mode = mode;

    // Add terminator and pad up to a byte if applicable
    uint32_t padding = (dataCapacity * 8) - codewords.bitOffsetOrWidth;
    if (padding > 4)
    {
        padding = 4;
    }
    bb_appendBits(&codewords, 0, padding);
    bb_appendBits(&codewords, 0, (8 - codewords.bitOffsetOrWidth % 8) % 8);

    // Pad with alternate bytes until data capacity is reached
    for (uint8_t padByte = 0xEC; codewords.bitOffsetOrWidth < (dataCapacity * 8); padByte ^= 0xEC ^ 0x11)
    {
        bb_appendBits(&codewords, padByte, 8);
    }

    BitBucket modulesGrid;
    bb_initGrid(&modulesGrid, modules, size);

    BitBucket isFunctionGrid;
    uint8_t isFunctionGridBytes[bb_getGridSizeBytes(size)];
    bb_initGrid(&isFunctionGrid, isFunctionGridBytes, size);

    // Draw function patterns, draw all codewords, do masking
    drawFunctionPatterns(&modulesGrid, &isFunctionGrid, version, eccFormatBits);
    performErrorCorrection(version, eccFormatBits, &codewords);
    drawCodewords(&modulesGrid, &isFunctionGrid, &codewords);

    uint8_t mask = 0;
    drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, mask);
    applyMask(&modulesGrid, &isFunctionGrid, mask);
    qrcode->mask = mask;

    return 0;
}

int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t ecc, const char *data)
{
    return qrcode_initBytes(qrcode, modules, version, ecc, (uint8_t *)data, strlen(data));
}

bool qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y)
{
    if (x >= qrcode->size || y >= qrcode->size)
    {
        return false;
    }

    uint32_t offset = y * qrcode->size + x;
    return (qrcode->modules[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
}

void qrcode_init(QRCode *qrcode, const char *data)
{
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(qrcode, qrcodeData, 3, ECC_LOW, data);
}