#ifndef GLSL_BASICFUNCTIONS_H
#define GLSL_BASICFUNCTIONS_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>
#include <QFileInfo>

#include <iostream>

inline void /*GLAPIENTRY */MessageCallback( GLenum source, GLenum type,
                                            GLuint id, GLenum severity,
                                            GLsizei length, const GLchar* message,
                                            const void* userParam ){
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM || severity == GL_DEBUG_SEVERITY_LOW) {
        std::string s_severity = (severity == GL_DEBUG_SEVERITY_HIGH ? "High" : severity == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" : "Low");
        std::cout << "Error " << id << " [severity=" << s_severity << "]: " << message << std::endl;
    }
}

inline bool checkOpenGLError(){
    bool error = false;
    int glErr = glGetError();
    while(glErr != GL_NO_ERROR)
    {
        std::cout << "[OpenGL] Error: " << glErr << std::endl;
        error = true;
        glErr = glGetError();
    }
    return !error;
}

inline bool printShaderErrors(GLuint shader,QOpenGLExtraFunctions* glFunctions){
    int state = 0;
    glFunctions->glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetShaderInfoLog(shader, len, &chWritten, log);
        std::cout << "[OpenGL] Shader error: " << log << std::endl;
        free(log);
    }
    return false;
}
inline bool printProgramErrors(int program,QOpenGLExtraFunctions* glFunctions){
    int state = 0;
    glFunctions->glGetProgramiv(program, GL_LINK_STATUS, &state);
    if (state == 1)
        return true;
    int len = 0;
    int chWritten = 0;
    char* log;
    glFunctions->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char*)malloc(len);
        glFunctions->glGetProgramInfoLog(program, len, &chWritten, log);
        std::cout << "[OpenGL] Program error: " << log << std::endl;
        free(log);
    }
    return false;
}

inline std::string readShaderSource(std::string filename){
    std::string content = "";
    QString qFilename = QString::fromStdString(filename);
    if (!QFile::exists(qFilename))
        qFilename = ":" + qFilename;
    if (!QFile::exists(qFilename)) {
        std::cerr << "The shader " << filename << " doesn't exist!" << std::endl;
        return "";
    }

    QFile file(qFilename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    std::string line;
    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine().toStdString();
        content += line + " \n";
    }
    file.close();

    //std::cout << content  << std::endl;
    return content;
}


inline void compileShaderProgram_VGF(QOpenGLContext** glContext, QOpenGLExtraFunctions** glFunctions, GLuint &programID, const char * v_path,const char * g_path, const char * f_path){
    QFileInfo info_vert(v_path);
    QFileInfo info_geom(g_path);
    QFileInfo info_frag(f_path);

    std::string vShaderPath = info_vert.absoluteFilePath().toStdString();
    std::string gShaderPath = info_geom.absoluteFilePath().toStdString();
    std::string fShaderPath = info_frag.absoluteFilePath().toStdString();

    *glContext = QOpenGLContext::currentContext();
    *glFunctions = (*glContext)->extraFunctions();
    glEnable              ( GL_DEBUG_OUTPUT );
    (*glFunctions)->glDebugMessageCallback(&MessageCallback, 0 );

    //=============== Create programs and link shaders
    programID = (*glFunctions)->glCreateProgram();
    std::string content = readShaderSource(vShaderPath);

    if (!content.empty()) {
         GLuint vShader = (*glFunctions)->glCreateShader(GL_VERTEX_SHADER);
         const char* src = content.c_str();
         (*glFunctions)->glShaderSource(vShader, 1, &src, NULL);
         (*glFunctions)->glCompileShader(vShader);
         (*glFunctions)->glAttachShader(programID, vShader);
         printShaderErrors(vShader,*glFunctions);
    }

    content = readShaderSource(gShaderPath);
    if (!content.empty()) {
         GLuint gShader = (*glFunctions)->glCreateShader(GL_GEOMETRY_SHADER);
         const char* src = content.c_str();
         (*glFunctions)->glShaderSource(gShader, 1, &src, NULL);
         (*glFunctions)->glCompileShader(gShader);
         (*glFunctions)->glAttachShader(programID, gShader);
         printShaderErrors(gShader,*glFunctions);
    }

    content = readShaderSource(fShaderPath);
    if (!content.empty()) {
         GLuint fShader = (*glFunctions)->glCreateShader(GL_FRAGMENT_SHADER);
         const char* src = content.c_str();
         (*glFunctions)->glShaderSource(fShader, 1, &src, NULL);
         (*glFunctions)->glCompileShader(fShader);
         (*glFunctions)->glAttachShader(programID, fShader);
         printShaderErrors(fShader,*glFunctions);
    }

    (*glFunctions)->glLinkProgram(programID);
    (*glFunctions)->glUseProgram(programID);
    printProgramErrors(programID,*glFunctions);
    checkOpenGLError();
}

inline void compileShaderProgram_VF(QOpenGLContext** glContext, QOpenGLExtraFunctions** glFunctions, GLuint &programID, const char * v_path, const char * f_path){
    QFileInfo info_vert(v_path);
    QFileInfo info_frag(f_path);

    std::string vShaderPath = info_vert.absoluteFilePath().toStdString();
    std::string fShaderPath = info_frag.absoluteFilePath().toStdString();



    *glContext = QOpenGLContext::currentContext();
    *glFunctions = (*glContext)->extraFunctions();
    glEnable              ( GL_DEBUG_OUTPUT );
    (*glFunctions)->glDebugMessageCallback(&MessageCallback, 0 );

    //=============== Create programs and link shaders
    programID = (*glFunctions)->glCreateProgram();
    std::string content = readShaderSource(vShaderPath);

    if (!content.empty()) {
         GLuint vShader = (*glFunctions)->glCreateShader(GL_VERTEX_SHADER);
         const char* src = content.c_str();
         (*glFunctions)->glShaderSource(vShader, 1, &src, NULL);
         (*glFunctions)->glCompileShader(vShader);
         (*glFunctions)->glAttachShader(programID, vShader);
         printShaderErrors(vShader,*glFunctions);
    }
    content = readShaderSource(fShaderPath);
    if (!content.empty()) {
         GLuint fShader = (*glFunctions)->glCreateShader(GL_FRAGMENT_SHADER);
         const char* src = content.c_str();
         (*glFunctions)->glShaderSource(fShader, 1, &src, NULL);
         (*glFunctions)->glCompileShader(fShader);
         (*glFunctions)->glAttachShader(programID, fShader);
         printShaderErrors(fShader,*glFunctions);
    }
    (*glFunctions)->glLinkProgram(programID);
    (*glFunctions)->glUseProgram(programID);

    printProgramErrors(programID,*glFunctions);
    checkOpenGLError();
}

#endif // GLSL_BASICFUNCTIONS_H
