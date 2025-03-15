import cv2
import numpy as np
from ultralytics import YOLO

# Load YOLOv8 model (use 'yolov8n.pt' or your custom trained model)
model = YOLO('yolo11n.pt')

# Open the camera
cap = cv2.VideoCapture(0)

def detect_light_color(roi):
    """Detect the color inside the traffic light bounding box."""
    hsv = cv2.cvtColor(roi, cv2.COLOR_BGR2HSV)

    # Define color ranges in HSV
    red_lower1 = np.array([0, 100, 100])
    red_upper1 = np.array([10, 255, 255])
    red_lower2 = np.array([160, 100, 100])
    red_upper2 = np.array([180, 255, 255])

    yellow_lower = np.array([20, 100, 100])
    yellow_upper = np.array([30, 255, 255])

    green_lower = np.array([40, 100, 100])
    green_upper = np.array([90, 255, 255])

    # Create masks
    red_mask = cv2.inRange(hsv, red_lower1, red_upper1) + cv2.inRange(hsv, red_lower2, red_upper2)
    yellow_mask = cv2.inRange(hsv, yellow_lower, yellow_upper)
    green_mask = cv2.inRange(hsv, green_lower, green_upper)

    # Count non-zero pixels in masks
    red_count = np.sum(red_mask)
    yellow_count = np.sum(yellow_mask)
    green_count = np.sum(green_mask)

    # Determine the dominant color
    if red_count > yellow_count and red_count > green_count:
        return "Red"
    elif yellow_count > red_count and yellow_count > green_count:
        return "Yellow"
    elif green_count > red_count and green_count > yellow_count:
        return "Green"
    else:
        return "Unknown"

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Perform object detection
    results = model(frame)

    # Process detections
    for result in results:
        for box in result.boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0])  # Bounding box
            conf = box.conf[0].item()  # Confidence score
            cls = int(box.cls[0])  # Class index
            label = model.names[cls]  # Get class label

            # Check if it's a traffic light
            if label == "traffic light":
                # Crop the region of interest (ROI)
                roi = frame[y1:y2, x1:x2]

                # Detect the color inside the ROI
                light_color = detect_light_color(roi)

                # Draw bounding box and label
                color_map = {"Red": (0, 0, 255), "Yellow": (0, 255, 255), "Green": (0, 255, 0)}
                cv2.rectangle(frame, (x1, y1), (x2, y2), color_map.get(light_color, (255, 255, 255)), 2)
                cv2.putText(frame, f"{light_color} ({conf:.2f})", (x1, y1 - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, color_map.get(light_color, (255, 255, 255)), 2)

    # Show the frame
    cv2.imshow('Traffic Light Detection', frame)

    # Exit on pressing 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
