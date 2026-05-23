//HERNANDEZ SANTOS KAREN CECILIA

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

// Clase Matrix: operaciones de algebra lineal
class Matrix {
private:
    int rows;                           // Numero de filas de la matriz
    int cols;                           // Numero de columnas de la matriz
    std::vector<std::vector<double>> data; // Contenedor bidimensional de los datos

public:
    // El constructor inicializa el tamaño y llena la matriz con ceros (0.0)
    Matrix(int r, int c) : rows(r), cols(c), data(r, std::vector<double>(c, 0.0)) {}

    // Metodos "Getter" para obtener las dimensiones de forma segura
    int getRows() const { return rows; }
    int getCols() const { return cols; }

    // Acceso a posiciones especificas (L-value y R-value para lectura/escritura)
    double& at(int r, int c) { return data[r][c]; }
    const double& at(int r, int c) const { return data[r][c]; }


    // Transpuesta de la matriz
    Matrix transpose() const {
        Matrix T(cols, rows); // La matriz transpuesta invierte dimensiones (cols x rows)
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                T.at(j, i) = data[i][j]; // Intercambio de índices
            }
        }
        return T;
    }


    // Multiplicacion de matrices
    Matrix multiply(const Matrix& other) const {
        // Regla del algebra lineal: las columnas de A deben ser iguales a las filas de B
        if (cols != other.getRows()) {
            throw std::runtime_error("Dimensiones incompatibles para multiplicacion.");
        }
        Matrix result(rows, other.getCols()); // Matriz resultante de tamaño (filas de A x columnas de B)

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.getCols(); ++j) {
                double sum = 0.0;
                for (int k = 0; k < cols; ++k) {
                    sum += data[i][k] * other.at(k, j); // Multiplicacion y acumulacion
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }


    // Inversa mediante eliminacion de Gauss-Jordan
    Matrix inverse() const {
        if (rows != cols) {
            throw std::runtime_error("La matriz no es cuadrada.");
        }
        int n = rows;
        Matrix aug(n, 2 * n); // Crea la estructura de la matriz aumentada [A | I]

        // Paso 1: Inicializar la matriz aumentada
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                aug.at(i, j) = data[i][j]; // Lado izquierdo: Matriz original
            }
            aug.at(i, i + n) = 1.0;        // Lado derecho: Matriz Identidad
        }

        // Paso 2: Algoritmo de Gauss-Jordan
        for (int i = 0; i < n; ++i) {
            // Pivoteo para evitar divisiones por cero y mejorar la estabilidad numerica
            int maxRow = i;
            for (int k = i + 1; k < n; ++k) {
                if (std::abs(aug.at(k, i)) > std::abs(aug.at(maxRow, i))) {
                    maxRow = k;
                }
            }
            if (maxRow != i) {
                std::swap(aug.data[i], aug.data[maxRow]); // Intercambia filas si encontro un pivote mayor
            }

            double pivot = aug.at(i, i);
            if (std::abs(pivot) < 1e-9) {
                throw std::runtime_error("La matriz es singular y no se puede invertir.");
            }

            // Normalizar la fila del pivote para que el elemento diagonal sea 1
            for (int j = 0; j < 2 * n; ++j) {
                aug.at(i, j) /= pivot;
            }

            // Hacer ceros en todas las demas celdas de la columna actual
            for (int k = 0; k < n; ++k) {
                if (k != i) {
                    double factor = aug.at(k, i);
                    for (int j = 0; j < 2 * n; ++j) {
                        aug.at(k, j) -= factor * aug.at(i, j);
                    }
                }
            }
        }

        // Paso 3: Extraer la matriz inversa calculada del lado derecho de la matriz aumentada
        Matrix inv(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inv.at(i, j) = aug.at(i, j + n);
            }
        }
        return inv;
    }


    void print(const std::string& name) const {
        std::cout << "--- Matriz " << name << " (" << rows << "x" << cols << ") ---" << std::endl;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                std::cout << std::setw(12) << std::fixed << std::setprecision(2) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};


// Clase MLRModel: Modelo de Regresion Lineal Multiple
class MLRModel {
private:
    Matrix X;      // Matriz de diseño (incluye la columna de unos para el intercepto B0)
    Matrix y;      // Vector columna de valores observados (Yield real)
    Matrix beta;   // Vector columna para almacenar los coeficientes optimos calculados
    bool isTrained;

public:
    // El constructor reserva espacio dinamico basado en las muestras y caracteristicas
    MLRModel(int samples, int features)
        : X(samples, features + 1), y(samples, 1), beta(features + 1, 1), isTrained(false) {}

    // Llena fila por fila la informacion de los experimentos
    void setSampleData(int index, double x1, double x2, double targetYield) {
        X.at(index, 0) = 1.0;         // Columna 0 siempre es 1 (coeficiente de Beta_0)
        X.at(index, 1) = x1;          // Columna 1: Variable predictora Factor 1
        X.at(index, 2) = x2;          // Columna 2: Variable predictora Factor 2
        y.at(index, 0) = targetYield; // Vector de respuestas esperadas
    }

    // Aplica la ecuacion de estimacion matricial
    void train() {
        Matrix XT = X.transpose();       // 1. Obtener X^T
        Matrix XTX = XT.multiply(X);     // 2. Calcular (X^T * X)

        // para comparar con los apuntes
        XTX.print("X'X");

        Matrix XTy = XT.multiply(y);     // 3. Calcular (X^T * y)
        XTy.print("X'y");

        Matrix XTX_inv = XTX.inverse();  // 4. Invertir: (X^T * X)^-1
        beta = XTX_inv.multiply(XTy);    // 5. Multiplicar final para obtener el vector Beta
        isTrained = true;                // Cambiar estado del modelo
    }

    // Simula predicciones usando los betas obtenidos: y_hat = B0 + B1*x1 + B2*x2
    double predict(double x1, double x2) const {
        if (!isTrained) {
            throw std::runtime_error("El modelo debe entrenarse antes de predecir.");
        }
        return beta.at(0, 0) + beta.at(1, 0) * x1 + beta.at(2, 0) * x2;
    }

    // Getter para exponer los coeficientes calculados de forma externa
    double getBeta(int index) const { return beta.at(index, 0); }
};


// Clase: ChemicalExperimentSimulator: Controlador del experimento
class ChemicalExperimentSimulator {
private:
    MLRModel model; // Instancia el modelo matemático interno

public:
    ChemicalExperimentSimulator() : model(17, 2) {
        // Datos del problema
        // Estructura: (indice de muestra, Factor 1, Factor 2, Yield)
        model.setSampleData(0,  41.9, 29.1, 251.3);
        model.setSampleData(1,  43.4, 29.3, 251.3);
        model.setSampleData(2,  43.9, 29.5, 248.3);
        model.setSampleData(3,  44.5, 29.7, 267.5);
        model.setSampleData(4,  47.3, 29.9, 273.0);
        model.setSampleData(5,  47.5, 30.3, 276.5);
        model.setSampleData(6,  47.9, 30.5, 270.3);
        model.setSampleData(7,  50.2, 30.7, 274.9);
        model.setSampleData(8,  52.8, 30.8, 285.0);
        model.setSampleData(9,  53.2, 30.9, 290.0);
        model.setSampleData(10, 56.7, 31.5, 297.0);
        model.setSampleData(11, 57.0, 31.7, 302.5);
        model.setSampleData(12, 63.5, 31.9, 304.5);
        model.setSampleData(13, 65.3, 32.0, 309.3);
        model.setSampleData(14, 71.1, 32.1, 321.7);
        model.setSampleData(15, 77.0, 32.5, 330.7);
        model.setSampleData(16, 77.8, 32.9, 349.0);
    }

    // Control central de la ejecucion del simulador
    void execute() {
        std::cout << "................................................................" << std::endl;
        std::cout << "    INICIANDO CALCULO DE PARAMETROS OPTIMOS (LEAST SQUARED) " << std::endl;
        std::cout << "................................................................\n" << std::endl;

        model.train(); // Dispara el procesamiento matricial

        std::cout << "................................................................\n" << std::endl;


        // Impresion de coeficientes finales y comparacion visual
        std::cout << "\n................................................................" << std::endl;
        std::cout << "                      RESULTADOS DEL MODELO                     " << std::endl;
        std::cout << "................................................................" << std::endl;
        std::cout << "Valores calculados de los parametros optimos:\n";
        std::cout << "Beta_0 (Intercepto): " << std::fixed << std::setprecision(2) << model.getBeta(0) << " (Esperado: -153.51)\n";
        std::cout << "Beta_1 (Factor 1):   " << std::fixed << std::setprecision(2) << model.getBeta(1) << " (Esperado: 1.24)\n";
        std::cout << "Beta_2 (Factor 2):   " << std::fixed << std::setprecision(2) << model.getBeta(2) << " (Esperado: 12.08)\n\n";

        // Despliegue formal de la ecuacion matematica sustituida
        std::cout << "Ecuacion de Regresion Polinomial / Multiple Ajustada:" << std::endl;
        std::cout << "  Yield^ = " << model.getBeta(0)
                  << " + (" << model.getBeta(1) << " * Factor_1) + ("
                  << model.getBeta(2) << " * Factor_2)" << std::endl;
        std::cout << "................................................................\n" << std::endl;

        runSimulations(); // Ejecuta las predicciones de simulacion
    }

private:
    // Genera por diseño 5 entornos de prueba con valores que no existian al inicio
    void runSimulations() {
        std::cout << "\n................................................................" << std::endl;
        std::cout << "       SIMULACION DE EXPERIMENTOS (VALORES DESCONOCIDOS)       " << std::endl;
        std::cout << "................................................................" << std::endl;
        std::cout << std::left << std::setw(15) << "Simulacion"
                  << std::setw(15) << "Factor 1 (x1)"
                  << std::setw(15) << "Factor 2 (x2)"
                  << "Yield Predicha (y^)" << std::endl;
        std::cout << "................................................................" << std::endl;

        // Pares ordenados {x1, x2} elegidos para simular sistemas
        std::vector<std::pair<double, double>> testScenarios = {
            {45.4, 28.5},
            {52.6, 30.3},
            {67.3, 31.4},
            {74.7, 33.2},
            {80.0, 34.7}
        };

        for (size_t i = 0; i < testScenarios.size(); ++i) {
            double x1 = testScenarios[i].first;
            double x2 = testScenarios[i].second;
            double predictedYield = model.predict(x1, x2); // Evaluacion de la hipotesis ajustada

            // Formateo alineado a la izquierda en columnas tabulares esteticas
            std::cout << std::left << std::setw(15) << (i + 1)
                      << std::setw(15) << std::fixed << std::setprecision(1) << x1
                      << std::setw(15) << std::fixed << std::setprecision(1) << x2
                      << std::fixed << std::setprecision(2) << predictedYield << std::endl;
        }
        std::cout << "................................................................" << std::endl;
    }
};


int main() {
    try {
        ChemicalExperimentSimulator simulator;
        simulator.execute();                   // Ejecucion segura de las subtareas
    } catch (const std::exception& e) {
        std::cerr << "Error en la ejecucion: " << e.what() << std::endl;
    }
    return 0; // Finalizacion del programa
}
