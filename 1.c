#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 哈夫曼树节点结构
typedef struct HuffmanNode {
    char data;              // 字符
    int weight;             // 权值（频率）
    struct HuffmanNode *left, *right, *next;
} HuffmanNode;

// 哈夫曼编码结构
typedef struct HuffmanCode {
    char data;              // 字符
    char *code;             // 对应的哈夫曼编码
} HuffmanCode;

// 创建哈夫曼树节点
HuffmanNode* createNode(char data, int weight) {
    HuffmanNode *node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->weight = weight;
    node->left = node->right = node->next = NULL;
    return node;
}

// 插入节点到有序链表中（按权值从小到大）
void insertNode(HuffmanNode **head, HuffmanNode *newNode) {
    if (*head == NULL || (*head)->weight > newNode->weight) {
        newNode->next = *head;
        *head = newNode;
    } else {
        HuffmanNode *current = *head;
        while (current->next != NULL && current->next->weight <= newNode->weight) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// 构建哈夫曼树
HuffmanNode* buildHuffmanTree(HuffmanNode **head, int n) {
    // 当链表中只有一个节点时，哈夫曼树构建完成
    while ((*head)->next != NULL) {
        // 取出权值最小的两个节点
        HuffmanNode *min1 = *head;
        HuffmanNode *min2 = (*head)->next;
        *head = min2->next;
        
        // 创建新节点，权值为两个最小节点权值之和
        HuffmanNode *newNode = createNode('\0', min1->weight + min2->weight);
        newNode->left = min1;
        newNode->right = min2;
        
        // 将新节点插入到有序链表中
        insertNode(head, newNode);
    }
    return *head;
}

// 生成哈夫曼编码
void generateHuffmanCodes(HuffmanNode *root, HuffmanCode *codes, int *index, char *code, int depth) {
    if (root == NULL) return;
    
    // 如果是叶子节点，保存编码
    if (root->left == NULL && root->right == NULL) {
        codes[*index].data = root->data;
        codes[*index].code = (char*)malloc((depth + 1) * sizeof(char));
        strcpy(codes[*index].code, code);
        (*index)++;
        return;
    }
    
    // 左子树编码为0
    if (root->left != NULL) {
        code[depth] = '0';
        code[depth + 1] = '\0';
        generateHuffmanCodes(root->left, codes, index, code, depth + 1);
    }
    
    // 右子树编码为1
    if (root->right != NULL) {
        code[depth] = '1';
        code[depth + 1] = '\0';
        generateHuffmanCodes(root->right, codes, index, code, depth + 1);
    }
}

// 查找字符的哈夫曼编码
char* findCode(HuffmanCode *codes, int n, char ch) {
    for (int i = 0; i < n; i++) {
        if (codes[i].data == ch) {
            return codes[i].code;
        }
    }
    return NULL;
}

// 编码字符串
char* encodeString(HuffmanCode *codes, int n, char *str) {
    int len = strlen(str);
    int totalLen = 1; // 包括结束符
    
    // 计算编码后的总长度
    for (int i = 0; i < len; i++) {
        char *code = findCode(codes, n, str[i]);
        if (code != NULL) {
            totalLen += strlen(code);
        }
    }
    
    // 分配内存并编码
    char *encoded = (char*)malloc(totalLen * sizeof(char));
    encoded[0] = '\0';
    
    for (int i = 0; i < len; i++) {
        char *code = findCode(codes, n, str[i]);
        if (code != NULL) {
            strcat(encoded, code);
        }
    }
    
    return encoded;
}

// 解码字符串
char* decodeString(HuffmanNode *root, char *encoded) {
    if (root == NULL || encoded == NULL) return NULL;
    
    int len = strlen(encoded);
    char *decoded = (char*)malloc((len + 1) * sizeof(char));
    int decodedIndex = 0;
    
    HuffmanNode *current = root;
    
    for (int i = 0; i < len; i++) {
        if (encoded[i] == '0') {
            current = current->left;
        } else if (encoded[i] == '1') {
            current = current->right;
        } else {
            printf("错误：编码包含非法字符 '%c'\n", encoded[i]);
            free(decoded);
            return NULL;
        }
        
        // 如果到达叶子节点
        if (current->left == NULL && current->right == NULL) {
            decoded[decodedIndex++] = current->data;
            current = root; // 回到根节点继续译码
        }
    }
    
    decoded[decodedIndex] = '\0';
    
    // 检查是否完整译码
    if (current != root) {
        printf("警告：编码不完整，可能无法正确译码\n");
    }
    
    return decoded;
}

// 释放哈夫曼树内存
void freeHuffmanTree(HuffmanNode *root) {
    if (root == NULL) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

// 释放编码表内存
void freeCodes(HuffmanCode *codes, int n) {
    for (int i = 0; i < n; i++) {
        free(codes[i].code);
    }
}

// 将字符串写入文件
int writeToFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("错误：无法创建文件 %s\n", filename);
        return 0;
    }
    fprintf(file, "%s", content);
    fclose(file);
    return 1;
}

// 从文件读取内容
char* readFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("错误：无法读取文件 %s\n", filename);
        return NULL;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 分配内存并读取内容
    char *content = (char*)malloc((fileSize + 1) * sizeof(char));
    fread(content, sizeof(char), fileSize, file);
    content[fileSize] = '\0';
    
    fclose(file);
    return content;
}

// 将二进制字符串转换为字节数据
unsigned char* binaryStringToBytes(const char *binaryStr, int *byteCount) {
    int bitCount = strlen(binaryStr);
    *byteCount = (bitCount + 7) / 8; // 计算需要的字节数
    
    unsigned char *bytes = (unsigned char*)malloc(*byteCount * sizeof(unsigned char));
    memset(bytes, 0, *byteCount);
    
    for (int i = 0; i < bitCount; i++) {
        if (binaryStr[i] == '1') {
            int byteIndex = i / 8;
            int bitIndex = 7 - (i % 8); // 高位在前
            bytes[byteIndex] |= (1 << bitIndex);
        }
    }
    
    return bytes;
}

// 将字节数据转换回二进制字符串
char* bytesToBinaryString(const unsigned char *bytes, int byteCount, int bitCount) {
    char *binaryStr = (char*)malloc(bitCount + 1);
    int index = 0;
    
    for (int i = 0; i < byteCount && index < bitCount; i++) {
        for (int j = 7; j >= 0 && index < bitCount; j--) {
            if (bytes[i] & (1 << j)) {
                binaryStr[index++] = '1';
            } else {
                binaryStr[index++] = '0';
            }
        }
    }
    
    binaryStr[bitCount] = '\0';
    return binaryStr;
}

// 压缩函数：将编码后的二进制字符串压缩为二进制文件
int compressToFile(const char *filename, const char *binaryStr, int originalSize, int *originalBitCount) {
    int bitCount = strlen(binaryStr);
    *originalBitCount = bitCount;
    int byteCount;
    unsigned char *bytes = binaryStringToBytes(binaryStr, &byteCount);
    
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("错误：无法创建压缩文件 %s\n", filename);
        free(bytes);
        return 0;
    }
    
    // 写入位数量（用于解压时知道准确的位数）
    fwrite(&bitCount, sizeof(int), 1, file);
    // 写入字节数据
    fwrite(bytes, sizeof(unsigned char), byteCount, file);
    
    fclose(file);
    free(bytes);
    
    printf("\n压缩统计信息：\n");
    printf("  原文件大小: %d 字节\n", originalSize);
    printf("  编码后位数: %d 位\n", bitCount);
    printf("  压缩后字节: %d 字节\n", byteCount + 4); // 加上4字节的bitCount
    printf("  压缩率: %.2f%%\n", (1 - (float)(byteCount + 4) / originalSize) * 100);
    printf("  存储空间节省: %d 字节\n", originalSize - (byteCount + 4));
    
    return 1;
}

// 解压函数：从二进制文件读取并恢复编码字符串
char* decompressFromFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("错误：无法读取压缩文件 %s\n", filename);
        return NULL;
    }
    
    // 读取位数量
    int bitCount;
    fread(&bitCount, sizeof(int), 1, file);
    
    // 计算需要的字节数
    int byteCount = (bitCount + 7) / 8;
    unsigned char *bytes = (unsigned char*)malloc(byteCount * sizeof(unsigned char));
    fread(bytes, sizeof(unsigned char), byteCount, file);
    
    fclose(file);
    
    // 将字节数据转换回二进制字符串
    char *binaryStr = bytesToBinaryString(bytes, byteCount, bitCount);
    free(bytes);
    
    return binaryStr;
}

// 保存哈夫曼树信息到文件（用于解压时重建哈夫曼树）
int saveHuffmanTreeInfo(const char *filename, char *chars, int *weights, int n) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("错误：无法创建哈夫曼树信息文件 %s\n", filename);
        return 0;
    }
    
    // 写入字符数量
    fprintf(file, "%d\n", n);
    
    // 写入字符和权值
    for (int i = 0; i < n; i++) {
        fprintf(file, "%c %d\n", chars[i], weights[i]);
    }
    
    fclose(file);
    return 1;
}

// 从文件读取哈夫曼树信息
int loadHuffmanTreeInfo(const char *filename, char **chars, int **weights, int *n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("错误：无法读取哈夫曼树信息文件 %s\n", filename);
        return 0;
    }
    
    // 读取字符数量
    fscanf(file, "%d", n);
    
    *chars = (char*)malloc((*n + 1) * sizeof(char));
    *weights = (int*)malloc(*n * sizeof(int));
    
    // 读取字符和权值
    for (int i = 0; i < *n; i++) {
        fscanf(file, " %c %d", &(*chars)[i], &(*weights)[i]);
    }
    (*chars)[*n] = '\0';
    
    fclose(file);
    return 1;
}

// 从文本文件统计字符频率
int countCharactersFromFile(const char *filename, char **chars, int **weights) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("错误：无法读取文件 %s\n", filename);
        return 0;
    }
    
    int freq[256] = {0};
    int ch;
    int totalChars = 0;
    
    // 统计字符频率
    while ((ch = fgetc(file)) != EOF) {
        if (ch >= 0 && ch < 256) {
            freq[ch]++;
            totalChars++;
        }
    }
    fclose(file);
    
    // 计算不同字符的数量
    int uniqueCount = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            uniqueCount++;
        }
    }
    
    // 分配内存并填充数据
    *chars = (char*)malloc((uniqueCount + 1) * sizeof(char));
    *weights = (int*)malloc(uniqueCount * sizeof(int));
    
    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            (*chars)[index] = (char)i;
            (*weights)[index] = freq[i];
            index++;
        }
    }
    (*chars)[uniqueCount] = '\0';
    
    printf("统计完成：\n");
    printf("  文件总字符数: %d\n", totalChars);
    printf("  不同字符数: %d\n", uniqueCount);
    
    if (uniqueCount < 50) {
        printf("  警告：字符种类少于50个，建议使用更大的测试文件\n");
    }
    if (totalChars < 500) {
        printf("  警告：总字符数少于500个，建议使用更大的测试文件\n");
    }
    
    return uniqueCount;
}

// 显示菜单
void showMenu() {
    printf("\n=========== 哈夫曼编译码系统 ===========\n");
    printf("1. 从文本文件统计并建立哈夫曼树\n");
    printf("2. 从键盘输入字符和权值\n");
    printf("3. 显示哈夫曼编码表\n");
    printf("4. 编码文本文件\n");
    printf("5. 解码编码文件\n");
    printf("6. 压缩编码文件\n");
    printf("7. 解压并解码文件\n");
    printf("8. 测试大文件（使用内置样本）\n");
    printf("0. 退出\n");
    printf("========================================\n");
    printf("请选择操作: ");
}

// 创建测试大文件
void createTestFile() {
    const char *testText = 
        "The quick brown fox jumps over the lazy dog. This sentence contains all letters of the English alphabet. "
        "However, it is relatively short. For better testing, we need a much longer text with more diverse characters. "
        "哈夫曼编码是一种用于数据压缩的熵编码算法。由David A. Huffman在1952年提出。该算法使用变长编码表对源符号进行编码，"
        "其中变长编码表是通过评估源符号出现概率的方法得到的。出现概率较高的字母使用较短的编码，反之出现概率较低的则使用较长的编码，"
        "从而使编码之后的字符串的平均长度降低，达到无损压缩数据的目的。"
        "1234567890!@#$%%^&*()_+-=[]{}|;:,.<>?/`~ 多种字符混合测试。\n"
        "This is a comprehensive test file for Huffman coding implementation. "
        "It includes English letters, Chinese characters, numbers, and special symbols. "
        "The file size should be large enough to meet the requirements of the scoring criteria. "
        "We need at least 500 characters and 50 different character types. "
        "Let's add more content to reach the required size. "
        "数据压缩是计算机科学中的重要主题，哈夫曼编码是其中基础且经典的算法。"
        "在信息论中，哈夫曼编码是最优的前缀编码方法之一。"
        "通过构建哈夫曼树，我们可以为每个字符分配唯一的二进制编码。"
        "编码的平均长度接近字符集的熵，从而实现高效压缩。"
        "This paragraph adds more English text to increase character diversity. "
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n";
    
    FILE *file = fopen("test_large.txt", "w");
    if (file) {
        // 写入多次以增加文件大小
        for (int i = 0; i < 5; i++) {
            fprintf(file, "%s", testText);
        }
        fclose(file);
        printf("已创建测试文件 test_large.txt\n");
        
        // 统计文件信息
        FILE *f = fopen("test_large.txt", "r");
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fclose(f);
        
        printf("文件大小: %ld 字节\n", size);
    } else {
        printf("错误：无法创建测试文件\n");
    }
}

int main() {
    int n = 0;
    HuffmanNode *head = NULL;
    HuffmanNode *huffmanTree = NULL;
    HuffmanCode *codes = NULL;
    char *chars = NULL;
    int *weights = NULL;
    char inputStr[1000];
    char *encoded = NULL;
    char *decoded = NULL;
    int choice;
    int originalBitCount = 0;
    
    printf("=== 哈夫曼编译码器 ===\n");
    printf("系统支持大文件处理（500+字符，50+字符种类）\n");
    
    do {
        showMenu();
        scanf("%d", &choice);
        getchar(); // 清除换行符
        
        switch(choice) {
            case 1: {
                // 从文件统计
                printf("请输入要统计的文本文件名: ");
                fgets(inputStr, sizeof(inputStr), stdin);
                inputStr[strcspn(inputStr, "\n")] = '\0';
                
                n = countCharactersFromFile(inputStr, &chars, &weights);
                if (n > 0) {
                    // 释放之前的树
                    if (huffmanTree) freeHuffmanTree(huffmanTree);
                    if (codes) freeCodes(codes, n);
                    if (codes) free(codes);
                    
                    // 创建节点并构建有序链表
                    head = NULL;
                    for (int i = 0; i < n; i++) {
                        HuffmanNode *newNode = createNode(chars[i], weights[i]);
                        insertNode(&head, newNode);
                    }
                    
                    // 构建哈夫曼树
                    huffmanTree = buildHuffmanTree(&head, n);
                    
                    // 生成哈夫曼编码
                    codes = (HuffmanCode*)malloc(n * sizeof(HuffmanCode));
                    char *tempCode = (char*)malloc((n + 1) * sizeof(char));
                    int index = 0;
                    tempCode[0] = '\0';
                    generateHuffmanCodes(huffmanTree, codes, &index, tempCode, 0);
                    free(tempCode);
                    
                    printf("哈夫曼树构建完成，共 %d 种字符\n", n);
                    
                    // 保存统计信息到文件
                    saveHuffmanTreeInfo("huffman_tree.txt", chars, weights, n);
                    printf("哈夫曼树信息已保存到 huffman_tree.txt\n");
                }
                break;
            }
            
            case 2: {
                // 从键盘输入
                printf("请输入字符集大小 n: ");
                scanf("%d", &n);
                getchar();
                
                if (chars) free(chars);
                if (weights) free(weights);
                chars = (char*)malloc((n + 1) * sizeof(char));
                weights = (int*)malloc(n * sizeof(int));
                
                printf("请输入 %d 个字符: ", n);
                for (int i = 0; i < n; i++) {
                    scanf("%c", &chars[i]);
                }
                chars[n] = '\0';
                getchar();
                
                printf("请输入 %d 个权值: ", n);
                for (int i = 0; i < n; i++) {
                    scanf("%d", &weights[i]);
                }
                getchar();
                
                // 释放之前的树
                if (huffmanTree) freeHuffmanTree(huffmanTree);
                if (codes) freeCodes(codes, n);
                if (codes) free(codes);
                
                // 创建节点并构建有序链表
                head = NULL;
                for (int i = 0; i < n; i++) {
                    HuffmanNode *newNode = createNode(chars[i], weights[i]);
                    insertNode(&head, newNode);
                }
                
                // 构建哈夫曼树
                huffmanTree = buildHuffmanTree(&head, n);
                
                // 生成哈夫曼编码
                codes = (HuffmanCode*)malloc(n * sizeof(HuffmanCode));
                char *tempCode = (char*)malloc((n + 1) * sizeof(char));
                int index = 0;
                tempCode[0] = '\0';
                generateHuffmanCodes(huffmanTree, codes, &index, tempCode, 0);
                free(tempCode);
                
                printf("哈夫曼树构建完成\n");
                break;
            }
            
            case 3: {
                if (codes == NULL || n == 0) {
                    printf("请先构建哈夫曼树！\n");
                } else {
                    printf("\n=== 字符哈夫曼编码表 ===\n");
                    printf("字符\t权值\t编码\n");
                    for (int i = 0; i < n; i++) {
                        printf("'%c'\t%d\t%s\n", codes[i].data, weights[i], codes[i].code);
                    }
                }
                break;
            }
            
            case 4: {
                if (codes == NULL || n == 0) {
                    printf("请先构建哈夫曼树！\n");
                    break;
                }
                
                printf("1. 编码文本文件\n");
                printf("2. 编码键盘输入的字符串\n");
                printf("请选择: ");
                int subChoice;
                scanf("%d", &subChoice);
                getchar();
                
                if (subChoice == 1) {
                    printf("请输入要编码的文本文件名: ");
                    fgets(inputStr, sizeof(inputStr), stdin);
                    inputStr[strcspn(inputStr, "\n")] = '\0';
                    
                    char *fileContent = readFromFile(inputStr);
                    if (fileContent) {
                        // 保存原始文件
                        writeToFile("SourceFile.txt", fileContent);
                        printf("原始字符串已保存到 SourceFile.txt\n");
                        
                        if (encoded) free(encoded);
                        encoded = encodeString(codes, n, fileContent);
                        printf("编码结果: %s\n", encoded);
                        
                        if (writeToFile("CodeFile.txt", encoded)) {
                            printf("编码结果已保存到 CodeFile.txt\n");
                        }
                        
                        free(fileContent);
                    }
                } else {
                    printf("请输入要编码的字符串: ");
                    fgets(inputStr, sizeof(inputStr), stdin);
                    inputStr[strcspn(inputStr, "\n")] = '\0';
                    
                    writeToFile("SourceFile.txt", inputStr);
                    printf("原始字符串已保存到 SourceFile.txt\n");
                    
                    if (encoded) free(encoded);
                    encoded = encodeString(codes, n, inputStr);
                    printf("编码结果: %s\n", encoded);
                    
                    if (writeToFile("CodeFile.txt", encoded)) {
                        printf("编码结果已保存到 CodeFile.txt\n");
                    }
                }
                break;
            }
            
            case 5: {
                if (huffmanTree == NULL) {
                    printf("请先构建哈夫曼树！\n");
                    break;
                }
                
                printf("1. 从CodeFile.txt解码\n");
                printf("2. 从键盘输入编码字符串\n");
                printf("请选择: ");
                int subChoice;
                scanf("%d", &subChoice);
                getchar();
                
                char *codeToDecode = NULL;
                
                if (subChoice == 1) {
                    codeToDecode = readFromFile("CodeFile.txt");
                    if (codeToDecode == NULL) {
                        printf("错误：无法读取CodeFile.txt\n");
                        break;
                    }
                } else {
                    printf("请输入要解码的编码字符串: ");
                    fgets(inputStr, sizeof(inputStr), stdin);
                    inputStr[strcspn(inputStr, "\n")] = '\0';
                    codeToDecode = strdup(inputStr);
                }
                
                if (decoded) free(decoded);
                decoded = decodeString(huffmanTree, codeToDecode);
                if (decoded != NULL) {
                    printf("译码结果: %s\n", decoded);
                    
                    if (writeToFile("DecodeFile.txt", decoded)) {
                        printf("译码结果已保存到 DecodeFile.txt\n");
                    }
                    
                    // 验证正确性
                    char *original = readFromFile("SourceFile.txt");
                    if (original) {
                        if (strcmp(original, decoded) == 0) {
                            printf("验证成功：译码结果与原始文件一致\n");
                        } else {
                            printf("验证失败：译码结果与原始文件不一致\n");
                        }
                        free(original);
                    }
                }
                
                if (subChoice == 1) free(codeToDecode);
                break;
            }
            
            case 6: {
                if (encoded == NULL) {
                    printf("请先编码一个字符串！\n");
                    break;
                }
                
                printf("压缩编码结果到二进制文件...\n");
                char *originalContent = readFromFile("SourceFile.txt");
                int originalSize = originalContent ? strlen(originalContent) : 0;
                free(originalContent);
                
                if (compressToFile("compressed.bin", encoded, originalSize, &originalBitCount)) {
                    printf("编码结果已压缩到 compressed.bin\n");
                }
                break;
            }
            
            case 7: {
                printf("从压缩文件解压并解码...\n");
                char *compressedEncoded = decompressFromFile("compressed.bin");
                if (compressedEncoded != NULL) {
                    printf("从 compressed.bin 解压的编码: %s\n", compressedEncoded);
                    
                    char *fileDecoded = decodeString(huffmanTree, compressedEncoded);
                    if (fileDecoded != NULL) {
                        printf("从压缩文件译码的结果: %s\n", fileDecoded);
                        
                        char *original = readFromFile("SourceFile.txt");
                        if (original) {
                            if (strcmp(original, fileDecoded) == 0) {
                                printf("压缩解压验证成功！\n");
                            } else {
                                printf("压缩解压验证失败！\n");
                            }
                            free(original);
                        }
                        
                        writeToFile("Decompressed.txt", fileDecoded);
                        printf("解压结果已保存到 Decompressed.txt\n");
                        
                        free(fileDecoded);
                    }
                    free(compressedEncoded);
                }
                break;
            }
            
            case 8: {
                printf("创建并测试大文件...\n");
                createTestFile();
                
                // 自动测试大文件
                n = countCharactersFromFile("test_large.txt", &chars, &weights);
                if (n > 0) {
                    // 构建哈夫曼树
                    head = NULL;
                    for (int i = 0; i < n; i++) {
                        HuffmanNode *newNode = createNode(chars[i], weights[i]);
                        insertNode(&head, newNode);
                    }
                    
                    huffmanTree = buildHuffmanTree(&head, n);
                    
                    // 生成编码
                    codes = (HuffmanCode*)malloc(n * sizeof(HuffmanCode));
                    char *tempCode = (char*)malloc((n + 1) * sizeof(char));
                    int index = 0;
                    tempCode[0] = '\0';
                    generateHuffmanCodes(huffmanTree, codes, &index, tempCode, 0);
                    free(tempCode);
                    
                    printf("大文件哈夫曼树构建完成\n");
                    
                    // 编码大文件
                    char *largeContent = readFromFile("test_large.txt");
                    if (largeContent) {
                        encoded = encodeString(codes, n, largeContent);
                        writeToFile("CodeFile_large.txt", encoded);
                        
                        // 解码验证
                        decoded = decodeString(huffmanTree, encoded);
                        if (decoded && strcmp(largeContent, decoded) == 0) {
                            printf("大文件编码译码验证成功！\n");
                        }
                        
                        free(largeContent);
                    }
                }
                break;
            }
            
            case 0: {
                printf("感谢使用哈夫曼编译码系统！\n");
                break;
            }
            
            default: {
                printf("无效选择，请重新输入！\n");
                break;
            }
        }
        
        if (choice != 0) {
            printf("\n按Enter键继续...");
            getchar();
        }
        
    } while (choice != 0);
    
    // 释放内存
    if (chars) free(chars);
    if (weights) free(weights);
    if (encoded) free(encoded);
    if (decoded) free(decoded);
    if (codes) {
        freeCodes(codes, n);
        free(codes);
    }
    if (huffmanTree) freeHuffmanTree(huffmanTree);
    
    return 0;
}