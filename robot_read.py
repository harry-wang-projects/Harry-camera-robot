from urllib.request import urlopen
import cv2
import numpy as np
import requests
from time import sleep

from keras.models import load_model  # TensorFlow is required for Keras to work

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("./teachable_model/keras_Model.h5", compile=False)

# Load the labels
class_names = open("./teachable_model/labels.txt", "r").readlines()


def url_to_image(url, readFlag=cv2.IMREAD_COLOR):
    # download the image, convert it to a NumPy array, and then read
    # it into OpenCV format
    resp = urlopen(url)
    image = np.asarray(bytearray(resp.read()), dtype="uint8")
    image = cv2.imdecode(image, readFlag)
    
    image = cv2.resize(image, (640, 480), interpolation = cv2.INTER_AREA)

    # return the image
    return image

while True:
    #response = requests.get("http://192.168.4.1/speed")
    #get_str = response.text
    get_str = 'N/A'
    print(get_str)
    get_key = cv2.waitKey(100);
    if get_key == ord('g'):
        break;
    elif get_key == ord('q'):
        requests.get('http://192.168.4.1/Q')
    elif get_key == ord('w'):
        requests.get('http://192.168.4.1/W')
    elif get_key == ord('e'):
        requests.get('http://192.168.4.1/E')
    elif get_key == ord('a'):
        requests.get('http://192.168.4.1/A')
    elif get_key == ord('s'):
        requests.get('http://192.168.4.1/S')
    elif get_key == ord('d'):
        requests.get('http://192.168.4.1/D')
    elif get_key == ord('z'):
        requests.get('http://192.168.4.1/Z')
    elif get_key == ord('x'):
        requests.get('http://192.168.4.1/X')
    elif get_key == ord('c'):
        requests.get('http://192.168.4.1/C')
    elif get_key == ord('v'):
        requests.get('http://192.168.4.1/V')
    got_pic = url_to_image("http://192.168.4.1/IMG");
    
    image = cv2.resize(got_pic, (224, 224), interpolation=cv2.INTER_AREA)
   
    # Make the image a numpy array and reshape it to the models input shape.
    image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)

    # Normalize the image array
    image = (image / 127.5) - 1

    # Predicts the model
    prediction = model.predict(image)
    index = np.argmax(prediction)
    class_name = class_names[index]
    confidence_score = prediction[0][index]

    # Print prediction and confidence score
    print("Class:", class_name[2:], end="")
    print("Confidence Score:", str(np.round(confidence_score * 100))[:-2], "%")    

    #pic, text, coords, font, fontscale, color, thickness, line
    got_pic = cv2.putText(got_pic, "Speed: ", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    got_pic = cv2.putText(got_pic, get_str, (120, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    got_pic = cv2.putText(got_pic, class_name[2:], (200, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    got_pic = cv2.putText(got_pic, str(np.round(confidence_score * 100))[:-2], (300, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
    cv2.imshow("hello", got_pic)
    sleep(0.1)
