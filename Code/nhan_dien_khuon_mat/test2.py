import cv2
import os
import numpy as np
from PIL import Image
import tkinter as tk
from tkinter import simpledialog, messagebox
from threading import Thread
import serial
import time
# Biến toàn cục để quản lý luồng và dừng chương trình
ser = serial.Serial('COM4', 115200)  # Thay 'COM4' bằng cổng của bạn
time.sleep(2)  # Chờ kết nối ổn định
running_thread = None
stop_event = False

def stop_program():
    global stop_event
    if running_thread and running_thread.is_alive():
        stop_event = True
        print("\n[INFO] Đang dừng chương trình...")
        running_thread.join()  # Đợi cho luồng kết thúc
        print("\n[INFO] Chương trình đã dừng.")
    else:
        print("\n[INFO] Không có chương trình nào đang chạy.")

def reset_stop_event():
    global stop_event
    stop_event = False

def run_function1(face_id):
    global stop_event
    reset_stop_event()

    cam = cv2.VideoCapture(0)
    cam.set(3, 640)
    cam.set(4, 480)

    face_detector = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
    print("\n [INFO] Khởi Tạo Camera ...")
    count = 0

    while not stop_event:
        ret, img = cam.read()
        img = cv2.flip(img, 1)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        faces = face_detector.detectMultiScale(gray, 1.1, 5)

        # Hiển thị thông báo lên khung hình
        cv2.putText(img, "Dua khuon mat vao khung hinh", (10, 30), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(img, f"Anh da chup: {count}", (10, 60), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2, cv2.LINE_AA)

        for (x, y, w, h) in faces:
            cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
            count += 1
            cv2.imwrite("dataset/User." + str(face_id) + '.' + str(count) + ".jpg", gray[y:y + h, x:x + w])

        cv2.imshow('image', img)
        k = cv2.waitKey(100) & 0xff
        if k == 27 or count >= 100:
            break

    print("\n [INFO] Thoát")
    cam.release()
    cv2.destroyAllWindows()


def start_function1():
    face_id = simpledialog.askstring("Nhập ID", "Vui lòng nhập ID khuôn mặt:")
    if not face_id:
        print("\n[INFO] Không nhập ID, dừng chương trình.")
        return
    global running_thread
    running_thread = Thread(target=run_function1, args=(face_id,))
    running_thread.start()

def run_function2():
    path = 'dataset'
    recognizer = cv2.face.LBPHFaceRecognizer_create()
    detector = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

    def getImagesAndLabels(path):
        imagePaths = [os.path.join(path, f) for f in os.listdir(path)]
        faceSamples = []
        ids = []

        for imagePath in imagePaths:
            PIL_img = Image.open(imagePath).convert('L')
            img_numpy = np.array(PIL_img, 'uint8')
            id = int(os.path.split(imagePath)[-1].split(".")[1])
            faces = detector.detectMultiScale(img_numpy)

            for (x, y, w, h) in faces:
                faceSamples.append(img_numpy[y:y + h, x:x + w])
                ids.append(id)

        return faceSamples, ids

    print("\n [INFO] Đang training dữ liệu ...")
    faces, ids = getImagesAndLabels(path)
    recognizer.train(faces, np.array(ids))
    recognizer.write('trainer/trainer.yml')
    print("\n [INFO] {0} khuôn mặt được train. Thoát.".format(len(np.unique(ids))))

def run_function3():
    recognizer = cv2.face.LBPHFaceRecognizer_create()
    recognizer.read('trainer/trainer.yml')
    faceCascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

    font = cv2.FONT_HERSHEY_SIMPLEX
    names = ['0', 'Tran Phuong Nam', '2', '3', '4', '5']

    cam = cv2.VideoCapture(0)
    cam.set(3, 640)
    cam.set(4, 480)
    minW = 0.1 * cam.get(3)
    minH = 0.1 * cam.get(4)
    # Biến để theo dõi thời gian gửi tín hiệu gần nhất
    last_sent_time = time.time()
    while not stop_event:
        ret, img = cam.read()
        img = cv2.flip(img, 1)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(int(minW), int(minH)))

        for (x, y, w, h) in faces:
            cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
            id, confidence = recognizer.predict(gray[y:y + h, x:x + w])

             # Nếu confidence nhỏ hơn 100, tức là nhận diện chính xác hơn
            if confidence < 100:
                id = names[id]
                confidence = "  {0}%".format(round(100 - confidence))

                # Chỉ gửi tín hiệu nếu đã quá 0.5 giây kể từ lần gửi cuối
                current_time = time.time()
                if current_time - last_sent_time > 0.5:
                    ser.write(b"ok\n")
                    print("Sent 'ok' to ESP32")
                    last_sent_time = current_time
            else:
                id = "unknown"
                confidence = "  {0}%".format(round(100 - confidence))
            # Hiển thị ID và độ tin cậy trên ảnh
            cv2.putText(img, str(id), (x+5, y-5), font, 1, (255, 255, 255), 2)
            cv2.putText(img, str(confidence), (x+5, y+h-5), font, 1, (255, 255, 255), 1)


        cv2.imshow('nhan dien khuon mat', img)
        k = cv2.waitKey(10) & 0xff
        if k == 27:
            break

    print("\n [INFO] Thoát")
    cam.release()
    cv2.destroyAllWindows()

def delete_face_data():
    face_id = simpledialog.askstring("Xóa dữ liệu", "Vui lòng nhập ID khuôn mặt để xóa:")
    if not face_id:
        print("\n[INFO] Không nhập ID, không thực hiện xóa.")
        return

    dataset_path = "dataset"
    files_deleted = 0

    if not os.path.exists(dataset_path):
        messagebox.showerror("Lỗi", "Thư mục dataset không tồn tại.")
        return

    for file in os.listdir(dataset_path):
        if file.startswith(f"User.{face_id}."):
            os.remove(os.path.join(dataset_path, file))
            files_deleted += 1

    if files_deleted > 0:
        messagebox.showinfo("Thành công", f"Đã xóa {files_deleted} tệp dữ liệu khuôn mặt với ID {face_id}.")
    else:
        messagebox.showinfo("Thông báo", f"Không tìm thấy dữ liệu khuôn mặt với ID {face_id}.")

def run_threaded_function(function):
    global running_thread
    running_thread = Thread(target=function)
    running_thread.start()

# Giao diện với tkinter
def create_interface():
    root = tk.Tk()
    root.title("Chương trình nhận diện khuôn mặt")
    root.geometry("300x300")

    btn1 = tk.Button(root, text="Chụp dữ liệu khuôn mặt", command=start_function1)
    btn1.pack(pady=10)

    btn2 = tk.Button(root, text="Train dữ liệu", command=lambda: run_threaded_function(run_function2))
    btn2.pack(pady=10)

    btn3 = tk.Button(root, text="Nhận diện khuôn mặt", command=lambda: run_threaded_function(run_function3))
    btn3.pack(pady=10)

    btn_delete = tk.Button(root, text="Xóa dữ liệu khuôn mặt", command=delete_face_data)
    btn_delete.pack(pady=10)

    btn_stop = tk.Button(root, text="Dừng chương trình", command=stop_program)
    btn_stop.pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    create_interface()
