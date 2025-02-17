import os
import subprocess,socketserver
import pathlib
import os.path as Path
from io import BytesIO
import sys
from dlsite_decrypter import dlsite_kts
from argparse import ArgumentParser
from dlsite_decrypter.dlst_package import DlstPackage

DLSITE_SERVICE_KEY = bytes.fromhex(
    "B1721A49C6AE503EC6117B8B562AB9BFA56E6A589181F90F45178CFFAD9A8B11"
)

class Message:
    def __init__(self,len1:int,len2:int,len3:int,data1:bytes,data2:bytes,data3:bytes):
        self.len1 = len1
        self.len2 = len2
        self.len3 = len3
        self.data1 = data1
        self.data2 = data2
        self.data3 = data3
    def unpack(data:bytes):
        with BytesIO(data) as br:
            signature = br.read(4)
            if(signature!=b"PACK"):
                raise Exception("Invalid signature, expected 'PACK'")
            len1=int.from_bytes(br.read(4),byteorder="little")
            len2=int.from_bytes(br.read(4),byteorder="little")
            len3=int.from_bytes(br.read(4),byteorder="little")
            data1 = br.read(len1)
            data2 = br.read(len2)
            data3 = br.read(len3)
            return Message(len1,len2,len3,data1,data2,data3)

def resource_path(relative_path):
    try:
        base_path = Path.abspath(sys._MEIPASS)
    except Exception:
        base_path = Path.abspath(".")
    return Path.join(base_path,relative_path)


def createHandler(dlst_path:str,output_directory:str):
    class MyHandler(socketserver.BaseRequestHandler):
        buffer:bytes = b""
        def handleMessage(self,msg:Message):
            key = msg.data1.decode("utf-8")
            if key =="LOG":
                print("[LOG]",msg.data2.decode("utf-8"))
            elif key =="KEY":
                print("[KEY]",msg.data2.hex(" "))
                decryptFile(dlst_path,msg.data2,output_directory)
                exit(0)
        def handle(self):
            try:
                while(True):
                    data = self.request.recv(1024)
                    if data == None:
                        break
                    self.buffer += data
                    while True:
                        buffer:bytes = self.buffer
                        if(len(buffer)<4):
                            break
                        if(buffer[:4]==b"PACK"):
                            len1=int.from_bytes(buffer[4:8],byteorder="little")
                            len2=int.from_bytes(buffer[8:12],byteorder="little")
                            len3=int.from_bytes(buffer[12:16],byteorder="little")
                            if len(buffer)>=(len1+len2+len3+17):
                                self.handleMessage(Message.unpack(buffer[:len1+len2+len3+17]))
                                self.buffer = buffer[len1+len2+len3+17:]
                            else:
                                break
                        else:
                            self.buffer = buffer[1:]
            except Exception:
                pass
    return MyHandler

def main():
    parser = ArgumentParser()
    parser.add_argument("dlst_file_path",type=str)
    parser.add_argument("dlsite_viewer_path",type=str)
    parser.add_argument("-o","--output-directory",required=False,type=str)
    args = parser.parse_args()
    output_directory = args.output_directory
    if not output_directory:
        output_directory = "."
    dlst_path = args.dlst_file_path
    if not Path.exists(dlst_path):
        print("dlst file does not exist!")
        return
    dlsite_viewer_path = args.dlsite_viewer_path
    if not Path.exists(dlsite_viewer_path):
        print("dlst viewer does not exist!")
        return
    if not Path.exists(output_directory):
        os.makedirs(output_directory,exist_ok=True)
    subprocess.run([resource_path("DllInjector.exe"),dlsite_viewer_path,resource_path("key_dumper.dll")])
    socketserver.TCPServer(("127.0.0.1",35701),createHandler(dlst_path,output_directory)).serve_forever()

def decryptFile(dlst_path:str,key:bytes,output_directory:str):
    with open(dlst_path, "rb") as (f):
        if dlsite_kts.is_file_package_from_file(f):
            p = DlstPackage.from_file(dlst_path)
        else:
            print("not a valid dlst file!")
            return
    dlsite_kts.add_package_svckey(p, DLSITE_SERVICE_KEY)
    p.key = key
    print(p.iv.hex(), p.key.hex())
    output_directory = Path.join(output_directory,pathlib.Path(dlst_path).stem)
    if not Path.exists(output_directory):
        os.makedirs(output_directory,exist_ok=True)
    for s, fn in dlsite_kts.export_package_content(p):
        save_path = Path.join(output_directory, fn)
        print(f"Saving file {Path.abspath(save_path)}")
        with open(save_path, "wb") as bw:
            bw.write(s)
if __name__ == "__main__":
    main()
