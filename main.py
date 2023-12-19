import cv2
import numpy as np
from serial import Serial

# Завантаження YOLO
net = cv2.dnn.readNet("yolov3.weights", "yolov3.cfg")
layer_names = net.getLayerNames()
output_layers = [layer_names[i - 1] for i in net.getUnconnectedOutLayers()]

# Відкриття відеофайлу
cap = cv2.VideoCapture("5.mp4")

period_count = 0

ser = Serial('/dev/ttyUSB0')

while True:
    # Зчитування кадру
    ret, frame = cap.read()
    if not ret:
        break

    height, width, channels = frame.shape

    # Переведення в RGB та нормалізація
    blob = cv2.dnn.blobFromImage(frame, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net.setInput(blob)
    outs = net.forward(output_layers)

    # Порог для виявлення об'єктів
    class_ids = []
    confidences = []
    boxes = []
    cares = 0
    for out in outs:

        for detection in out:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]
            if confidence > 0.0001 and class_id == 2:  # Ідентифікатор класу для автомобілів у YOLO
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)

                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                boxes.append([x, y, w, h])
                confidences.append(float(confidence))
                class_ids.append(class_id)
    # Використання non-maximum suppression для позбавлення від дублікатів
    
    indexes = cv2.dnn.NMSBoxes(boxes, confidences, 0.3, 0.4)

    # Виведення результату
	
    font = cv2.FONT_HERSHEY_PLAIN
    cv2.putText(frame, f'Kiko car: {len(indexes)}', (10, 50), font, 2, (0, 255, 0), 2)
    for i in indexes:
            
            x, y, w, h = boxes[i]
            label = str(class_ids[i])
            confidence = confidences[i]
            color = (0, 255, 0)
            cv2.rectangle(frame, (x, y), (x + w, y + h), color, 2)
            cv2.putText(frame, label + " " + str(round(confidence, 2)), (x, y + 30), font, 1, color, 2)

    period_count += 1
    if period_count == 20:
         period_count = 0
         ser.write(str(len(indexes)).encode())

    
    # Показ кадру
    cv2.imshow("Video", frame)

    # Вихід з циклу, якщо натиснута клавіша 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Закриття відеопотоку та вікон
cap.release()
cv2.destroyAllWindows()
ser.close()

