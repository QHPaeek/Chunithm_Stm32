#include <stdio.h>
#include <math.h>

void generateGammaTable(float gamma, unsigned char gammaTable[256]) {
    for (int i = 0; i < 256; i++) {
        gammaTable[i] = (unsigned char)(pow((i / 255.0), gamma) * 255.0);
    }
}

int main() {
    float gamma = 1.75; // 预设的伽马值
    unsigned char gammaTable[256];

    generateGammaTable(gamma, gammaTable);

    // 打印生成的伽马校正表
    printf("const uint8_t gamma8[256] = {\n");
    for (int i = 0; i < 256; i++) {
        printf("%3d", gammaTable[i]);
        if (i < 255) {
            printf(", ");
        }
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("};\n");
    getch();
    return 0;
}
