import socket

class Client:
    def __init__(self):
        self.server = None
        self.addr = None
        self.buffer = ""
        self.activo = True
        self.puerto = 0

    def init(self):
        self.activo = True

        self.puerto = int(input("Ingrese el puerto al que quiere conectarse: "))
        print("Conectando al servidor...\n\n")

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.addr = ('192.168.1.43', self.puerto)

        while True:
            try:
                self.server.connect(self.addr)
                break
            except:
                print("Ha ingresado mal el puerto")
                self.puerto = int(input("Ingrese un puerto nuevamente: "))
                self.addr = ('192.168.1.43', self.puerto)

        print("Conectado al Servidor!\n\n")

    def enviar(self):
        salir = False

        while not salir:
            print("Por favor, ingrese una de las siguientes opciones")
            print("Opcion 1: Generar nombre de usuario")
            print("Opcion 2: Generar contrasenia")
            print("Opcion 3: Cerrar sesion")
            opcion = input()

            if len(opcion) > 1:
                opcion = '4'  # opción inválida

            if opcion == '1':
                print("Ingrese la longitud deseada: (Entre 5 y 15)")
                while True:
                    mensaje = input()
                    if mensaje == "volver":
                        break
                    if 5 <= int(mensaje) <= 15:
                        print("Longitud aceptada:\n")
                        break
                    else:
                        print("Longitud invalida. Intente nuevamente.")

                mensaje = f"a{mensaje}"
                self.buffer = mensaje
                salir = True

            elif opcion == '2':
                print("Ingrese la longitud deseada: (Entre 8 inclusive y 50)")
                while True:
                    mensaje = input()
                    if mensaje == "volver":
                        break
                    if 8 <= int(mensaje) < 50:
                        print("Longitud aceptada:\n")
                        break
                    else:
                        print("Longitud invalida. Intente nuevamente.")

                mensaje = f"b{mensaje}"
                self.buffer = mensaje
                salir = True

            elif opcion == '3':
                self.buffer = "c"
                self.server.send(self.buffer.encode())
                self.cerrar_socket()
                self.activo = False
                return

            else:
                print("Error, ha ingresado un caracter incorrecto\n\n")

        self.server.send(self.buffer.encode())
        self.buffer = ""
        print("El mensaje ha sido enviado.")

    def recibir(self):
        data = self.server.recv(1024).decode()
        print("\nEl servidor dice: \n" + data)
        return data

    def cerrar_socket(self):
        self.server.close()

def main():
    cliente = Client()
    cliente.init()

    while cliente.activo:
        cliente.enviar()
        if cliente.activo:
            if cliente.recibir() == -1:
                cliente.init()

    print("Se ha cerrado la conexion.\n")

if __name__ == "__main__":
    main()