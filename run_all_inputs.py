import os
import subprocess
import shutil

# Archivos C++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print(" Compilación exitosa\n")

input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

#Limpiar archivos previos
for f in os.listdir(input_dir):
    if f.endswith(".s"):
        os.remove(os.path.join(input_dir, f))

for f in os.listdir(output_dir):
    if f.endswith(".s"):
        os.remove(os.path.join(output_dir, f))

# Ejecutar inputs
for i in range(1, 19):
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if os.path.isfile(filepath):
        print(f"Ejecutando {filename}")
        run_cmd = ["./a.out", filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f" Error ejecutando {filename}:\n{result.stderr}")
            continue

        tokens_file = os.path.join(input_dir, f"input{i}.s")

        if os.path.isfile(tokens_file):
            dest_tokens = os.path.join(output_dir, f"input_{i}.s")
            shutil.move(tokens_file, dest_tokens)
            print(f"    Generado: {dest_tokens}")
        else:
            print(f"  No se generó archivo .s para {filename}")
    else:
        print(f"{filename} no encontrado en {input_dir}")

print("\n Ejecución completada.")
