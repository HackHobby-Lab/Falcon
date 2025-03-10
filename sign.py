import cv2
import torch
import numpy as np
from picamera2 import Picamera2
# Load the YOLOv5 model (pre-trained on COCO dataset or custom road sign model)
model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)

# Open the Raspberry Pi Camera (or USB Camera)
# cap = cv2.VideoCapture(0)  # Change to `1` if using an external webcam

# Initialize camera
cap = Picamera2()
cap.start()
cv2.namedWindow("Camera Feed", cv2.WINDOW_NORMAL)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Convert frame to RGB (YOLO expects RGB input)
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Perform object detection
    results = model(frame_rgb)

    # Process the detections
    for det in results.xyxy[0]:  # Bounding boxes
        x1, y1, x2, y2, conf, cls = det
        label = results.names[int(cls)]

        # Filter for road signs (you may need a custom model for specific signs)
        if label in ["stop sign", "traffic light", "speed limit"]:
            # Draw bounding box
            cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 2)
            cv2.putText(frame, f"{label} ({conf:.2f})", (int(x1), int(y1) - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Show the frame
    cv2.imshow('Road Sign Detection', frame)

    # Exit on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
