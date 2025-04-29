# tarea 1 logaritmos


#TODO:

1) Generar una funcion recursiva de la forma:

 def sort(N,M,B,nombre_archivo):
    # N tamaño input
    # M tamaño RAM
    # B tamaño del bloque

    Elegir pivotes segun aridad (ej: a=2 --> elegir un pivote para dividir el input en 2)

    Si el numero de bloques cae en RAM:
        leer bloques y ordenar en RAM
    Si no cae en RAM:
        entonces recursivamente aplicar la funcion en los segmentos generados por el pivote
    
    ##despues de vuelta concatenar los segmentos ordenados (??)
