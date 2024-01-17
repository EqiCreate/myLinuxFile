import fitz  # PyMuPDF
import cv2
import pytesseract
from PIL import Image
from your_object_detection_model import detect_tables

# 加载PDF文件
pdf = fitz.open('1.pdf')

# 遍历每一页
for page_number in range(len(pdf)):
    # 将PDF页面转换为图像
    page = pdf.load_page(page_number)
    pix = page.get_pixmap()
    image_path = f'temp_page_{page_number}.png'
    pix.save(image_path)

    # 读取图像
    image = cv2.imread(image_path)

    # 检测表格边框
    tables = detect_tables(image)

    # 对于每个检测到的表格
    for table in tables:
        x, y, w, h = table

        # 提取表格区域
        table_region = image[y:y+h, x:x+w]

        # 使用OCR提取文字
        text = pytesseract.image_to_string(table_region)

        # 输出或处理提取到的文字
        print(text)

pdf.close()
