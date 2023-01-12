#ifndef GLSL_BASICFUNCTIONS_H
#define GLSL_BASICFUNCTIONS_H

#include <QOpenGLExtraFunctions>
#include <QGLViewer/vec.h>
#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>
#include <QFileInfo>

#include <iostream>

inline namespace GLTools{
    enum MaterialStandard {
        MY_MATERIAL_01 ,
        TAMY_MATERIAL_01 ,
        TAMY_MATERIAL ,
        EMERALD ,
        JADE ,
        OBSIDIAN ,
        PEARL ,
        RUBY ,
        TURQUOISE ,
        BRASS ,
        BRONZE ,
        CHROME ,
        COPPER ,
        GOLD ,
        POLISHED_GOLD ,
        SILVER ,
        POLISHED_SILVER ,
        BLACK_PLASTIC ,
        CYAN_PLASTIC ,
        GREEN_PLASTIC ,
        RED_PLASTIC ,
        WHITE_PLASTIC ,
        YELLOW_PLASTIC ,
        BLACK_RUBBER ,
        CYAN_RUBBER ,
        GREEN_RUBBER ,
        RED_RUBBER ,
        WHITE_RUBBER ,
        YELLOW_RUBBER ,
        PEWETER
    };

    inline qglviewer::Vec pointLightColors[] = {
        qglviewer::Vec(0.8f, 0.8f, 0.8f),
        qglviewer::Vec(0.8f, 0.8f, 0.8f),
        qglviewer::Vec(0.8f, 0.8f, 0.8f),
        qglviewer::Vec(0.8f, 0.8f, 0.8f),

        qglviewer::Vec(1.0f,0.0f, 0.0f),
        qglviewer::Vec(0.0f, 1.0f, 0.0f),
        qglviewer::Vec(0.0f, 0.0f, 1.0f),
        qglviewer::Vec(1.0f, 1.0f, 1.0f),
        qglviewer::Vec(0.28f, 0.39f, 1.0f),
        qglviewer::Vec(1.0f, 0.69f, 0.23f)
    };

    inline qglviewer::Vec pointLightPositions[] = {
        //Todo changer en coord normalisé
        qglviewer::Vec(1500.0f, 0.0f, 0.0f),
        qglviewer::Vec(0.0f, 1500.0f, 0.0f),
        qglviewer::Vec(0.0f, 0.0f, 1500.0f),
        qglviewer::Vec(1500.0f, 1500.0f, 1500.0f),

        qglviewer::Vec(0.0f, 0.0f, 10.0f),
        qglviewer::Vec(52.0f, 16.0f, 50.0f),
        qglviewer::Vec(26.0f, 48.0f, 50.0f),
        qglviewer::Vec(-16.0f, 52.0f, 50.0f),
        qglviewer::Vec(42.0f, 374.0f, 161.0f),
        qglviewer::Vec(473.0f, -351.0f, -259.0f),
        qglviewer::Vec(-438.0f, 167.0f, -48.0f)
    };

    inline qglviewer::Vec pointLightSpecularity[] = {
        qglviewer::Vec(0.8f, 0.8f,0.8f)
    };

    inline qglviewer::Vec pointLightAmbient[] = {
        qglviewer::Vec(0.3f, 0.3f, 0.3f)
    };

    inline const char* SHADERS_name_pointL             = "pointLights";
    inline const char* SHADERS_name_pointLposition     = "position";
    inline const char* SHADERS_name_pointLambient      = "ambient";
    inline const char* SHADERS_name_pointLdiffuse      = "diffuse";
    inline const char* SHADERS_name_pointLspecular     = "specular";
    inline const char* SHADERS_name_pointLconstant     = "constant";
    inline const char* SHADERS_name_pointLlinear       = "linear";
    inline const char* SHADERS_name_pointLquadratic    = "quadratic";

    inline const char* SHADERS_name_ambient     = "u_ambient";
    inline const char* SHADERS_name_diffuse     = "u_diffuse";
    inline const char* SHADERS_name_specular    = "u_specular";
    inline const char* SHADERS_name_shininess   = "u_shininess";
    inline const char* SHADERS_name_nbpL        = "nb_pointlight";

    inline std::string getStrPLightElement(int nb_ptl, const char* element){
        std::stringstream s;
        s << SHADERS_name_pointL << "[" << nb_ptl << "]." << element;
        return s.str();
    }

    inline void setPointLight(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions, int nb_ptl, qglviewer::Vec position,float constant,float linear,float quadratic,qglviewer::Vec ambient,qglviewer::Vec diffuse, qglviewer::Vec specular){
        std::cout << "SET PL : " << std::endl;
        std::cout << "pos      : " << position[0] << "," << position[1] << "," << position[2] << std::endl;
        std::cout << "ambient  : " << ambient[0] << "," << ambient[1] << "," << ambient[2] << std::endl;
        std::cout << "diffuse  : " << diffuse[0] << "," << diffuse[1] << "," << diffuse[2] << std::endl;
        std::cout << "specular : " << specular[0] << "," << specular[1] << "," << specular[2] << std::endl;

        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLposition).c_str()),
                                     position[0], position[1], position[2]);

        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLambient).c_str()),
                                     ambient[0],  ambient[1],  ambient[2]);

        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLdiffuse).c_str()),
                                     diffuse[0], diffuse[1],diffuse[2]);

        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLspecular).c_str()),
                                     specular[0],  specular[1],  specular[2]);

        cur_glFunctions->glUniform1f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLconstant).c_str()),constant);
        cur_glFunctions->glUniform1f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLlinear).c_str()), linear);
        cur_glFunctions->glUniform1f(cur_glFunctions->glGetUniformLocation(program, getStrPLightElement(nb_ptl, SHADERS_name_pointLquadratic).c_str()), quadratic);
    }
    inline void setAmbient(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions,float r, float g , float b){
        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program,SHADERS_name_ambient),r,g,b);
    }
    inline void setDiffuse(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions,float r, float g , float b){
        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program,SHADERS_name_diffuse),r,g,b);
    }
    inline void setSpecular(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions,float r, float g , float b){
        cur_glFunctions->glUniform3f(cur_glFunctions->glGetUniformLocation(program,SHADERS_name_specular),r,g,b);
    }
    inline void setShininess(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions,float sh){
        cur_glFunctions->glUniform1f(cur_glFunctions->glGetUniformLocation(program,SHADERS_name_shininess),sh);
    }

    inline void initLightsDefault(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions) {
        setPointLight(program, cur_glFunctions,0,pointLightPositions[0],1.0,0.09,0.032,pointLightColors[0],pointLightColors[0],pointLightColors[0]);
        setPointLight(program, cur_glFunctions,1,pointLightPositions[1],1.0,0.09,0.032,pointLightColors[1],pointLightColors[1],pointLightColors[1]);
        setPointLight(program, cur_glFunctions,2,pointLightPositions[2],1.0,0.09,0.032,pointLightColors[2],pointLightColors[2],pointLightColors[2]);
        setPointLight(program, cur_glFunctions,3,pointLightPositions[3],1.0,0.09,0.032,pointLightColors[3],pointLightColors[3],pointLightColors[3]);

        cur_glFunctions->glUniform1i(cur_glFunctions->glGetUniformLocation(program,SHADERS_name_nbpL),4);
    }

    inline void MLoadStandard(GLuint program, QOpenGLExtraFunctions*  cur_glFunctions, GLTools::MaterialStandard typ )
    {
        switch( typ ) {
            // My special materials
        case MY_MATERIAL_01:
            //setAmbient  (program,cur_glFunctions, 0.0f,			0.058823530f,	0.32549021f );
            setAmbient  (program,cur_glFunctions, .3f, .3f, .3f );
            setDiffuse  (program,cur_glFunctions, .7f, .7f, .7f );
            //setDiffuse  (program,cur_glFunctions, 0.54509807f,	0.76078433f,	0.81960785f );
            setSpecular (program,cur_glFunctions, 1.f,			1.f,			1.f );
            setShininess(program,cur_glFunctions, 1.f );
            break;
            // Tamy material
        case TAMY_MATERIAL_01:
            setAmbient  (program,cur_glFunctions, 0.f,			0.f,			0.f );
            setDiffuse  (program,cur_glFunctions, 1.f,			1.f,			1.f );
            setSpecular (program,cur_glFunctions, 0.5f,			0.5f,			0.5f );
            setShininess(program,cur_glFunctions, 1.f );
            break;
            // Tamy material
        case TAMY_MATERIAL:
            setAmbient  (program,cur_glFunctions, 0.f,			0.f,			0.f );
            setDiffuse  (program,cur_glFunctions, 1.f,			1.f,			1.f );
            setSpecular (program,cur_glFunctions, 0.01f,		.01f,			0.01f );
            setShininess(program,cur_glFunctions, 0.01f );
            break;

        case EMERALD:
            setAmbient  (program,cur_glFunctions, 0.0215f,		0.1745f,		0.0215f );
            setDiffuse  (program,cur_glFunctions, 0.07568f,		0.61424f,		0.07568f );
            setSpecular (program,cur_glFunctions, 0.633f,		0.727811f,		0.0633f );
            setShininess(program,cur_glFunctions, 0.6f );
            break;
        case JADE:
            setAmbient  (program,cur_glFunctions, 0.135f,       0.2225f,		0.1575f );
            setDiffuse  (program,cur_glFunctions, 0.54f,		0.89f,			0.063f );
            setSpecular (program,cur_glFunctions, 0.316228f,	0.0316228f,		0.316228f );
            setShininess(program,cur_glFunctions, 0.1f );
            break;
        case OBSIDIAN:
            setAmbient  (program,cur_glFunctions, 0.05375f,     0.05f,			0.0625f );
            setDiffuse  (program,cur_glFunctions, 0.18275f,     0.17f,			0.22525f );
            setSpecular (program,cur_glFunctions, 0.332741f,	0.328634f,		0.346435f );
            setShininess(program,cur_glFunctions, 0.3f );
            break;
        case PEARL:
            setAmbient  (program,cur_glFunctions, 0.25f,		0.20725f,		0.20725f );
            setDiffuse  (program,cur_glFunctions, 1.0f,         0.829f,			0.829f );
            setSpecular (program,cur_glFunctions, 0.296648f,	0.296648f,		0.296648f );
            setShininess(program,cur_glFunctions, 0.088f );
            break;
        case RUBY:
            setAmbient  (program,cur_glFunctions, 0.1745f,      0.01175f,		0.01175f );
            setDiffuse  (program,cur_glFunctions, 0.61424f,     0.04136f,		0.04136f );
            setSpecular (program,cur_glFunctions, 0.727811f,	0.626959f,		0.626959f );
            setShininess(program,cur_glFunctions, 0.6f );
            break;
        case TURQUOISE:
            setAmbient  (program,cur_glFunctions, 0.1f,         0.18725f,		0.1745f );
            setDiffuse  (program,cur_glFunctions, 0.396f,       0.74151f,		0.69102f );
            setSpecular (program,cur_glFunctions, 0.297254f,	0.30829f,		0.306678f );
            setShininess(program,cur_glFunctions, 0.1f );
            break;
        case BRASS:
            setAmbient  (program,cur_glFunctions, 0.329412f,	0.223529f,		0.027451f );
            setDiffuse  (program,cur_glFunctions, 0.780392f,	0.568627f,		0.113725f );
            setSpecular (program,cur_glFunctions, 0.992157f,	0.941176f,		0.807843f );
            setShininess(program,cur_glFunctions, 0.21794872f );
            break;
        case BRONZE:
            setAmbient  (program,cur_glFunctions, 0.2125f,      0.1275f,		0.054f );
            setDiffuse  (program,cur_glFunctions, 0.714f,       0.4284f,		0.18144f );
            setSpecular (program,cur_glFunctions, 0.393548f,	0.271906f,		0.166721f );
            setShininess(program,cur_glFunctions, 0.2f );
            break;
        case CHROME:
            setAmbient  (program,cur_glFunctions, 0.25f,		0.25f,			0.25f );
            setDiffuse  (program,cur_glFunctions, 0.4f,         0.4f,			0.4f );
            setSpecular (program,cur_glFunctions, 0.774597f,	0.774597f,		0.774597f );
            setShininess(program,cur_glFunctions, 0.6f );
            break;
        case COPPER:
            setAmbient  (program,cur_glFunctions, 0.19125f,     0.0735f,		0.0225f );
            setDiffuse  (program,cur_glFunctions, 0.7038f,  	0.27048f,		0.0828f );
            setSpecular (program,cur_glFunctions, 0.256777f,	0.137622f,		0.086014f );
            setShininess(program,cur_glFunctions, 0.1f );
            break;
        case GOLD:
            setAmbient  (program,cur_glFunctions, 0.24725f,     0.1995f,		0.0745f );
            setDiffuse  (program,cur_glFunctions, 0.75164f,     0.60648f,		0.22648f );
            setSpecular (program,cur_glFunctions, 0.628281f,	0.555802f,		0.366065f );
            setShininess(program,cur_glFunctions, 0.4f );
            break;
        case POLISHED_GOLD:
            setAmbient  (program,cur_glFunctions, 0.24725f,     0.2245f,		0.0645f );
            setDiffuse  (program,cur_glFunctions, 0.34615f,     0.3143f,		0.0903f );
            setSpecular (program,cur_glFunctions, 0.797357f,	0.723991f,		0.208006f );
            setShininess(program,cur_glFunctions, 83.2f / 128.f );
            break;
        case SILVER:
            setAmbient  (program,cur_glFunctions, 0.19225f,     0.19225f,		0.19225f );
            setDiffuse  (program,cur_glFunctions, 0.50754f,     0.50754f,		0.50754f );
            setSpecular (program,cur_glFunctions, 0.508273f,	0.508273f,		0.508273f );
            setShininess(program,cur_glFunctions, 0.4f );
            break;
        case POLISHED_SILVER:
            setAmbient  (program,cur_glFunctions, 0.23125f,     0.23125f,		0.23125f );
            setDiffuse  (program,cur_glFunctions, 0.2775f,     0.2775f,		0.2775f );
            setSpecular (program,cur_glFunctions, 0.773911f,	0.773911f,		0.773911f );
            setShininess(program,cur_glFunctions, 89.6f / 128.f );
            break;
        case BLACK_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.01f,		0.01f,			0.01f );
            setSpecular (program,cur_glFunctions, 0.50f,		0.50f,			0.50f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case CYAN_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.1f,			0.06f );
            setDiffuse  (program,cur_glFunctions, 0.0f,			0.50980392f,	0.50980392f );
            setSpecular (program,cur_glFunctions, 0.50196078f,	0.50196078f,	0.50196078f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case GREEN_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.1f,			0.35f,			0.1f );
            setSpecular (program,cur_glFunctions, 0.45f,		0.55f,			0.45f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case RED_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.5f,			0.0f,			0.0f );
            setSpecular (program,cur_glFunctions, 0.7f,			0.6f,			0.6f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case WHITE_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.55f,		0.55f,			0.55f );
            setSpecular (program,cur_glFunctions, 0.70f,		0.70f,			0.70f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case YELLOW_PLASTIC:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.5f,			0.5f,			0.0f );
            setSpecular (program,cur_glFunctions, 0.6f,			0.6f,			0.5f );
            setShininess(program,cur_glFunctions, 0.25f );
            break;
        case BLACK_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.02f,		0.02f,			0.02f );
            setDiffuse  (program,cur_glFunctions, 0.01f,		0.01f,			0.01f );
            setSpecular (program,cur_glFunctions, 0.4f,			0.4f,			0.4f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case CYAN_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.05f,			0.05f );
            setDiffuse  (program,cur_glFunctions, 0.4f,			0.5f,			0.5f );
            setSpecular (program,cur_glFunctions, 0.04f,		0.7f,			0.7f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case GREEN_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.0f,			0.05f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.4f,			0.5f,			0.4f );
            setSpecular (program,cur_glFunctions, 0.04f,		0.7f,			0.04f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case RED_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.05f,        0.0f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.5f,			0.4f,			0.4f );
            setSpecular (program,cur_glFunctions, 0.7f,			0.04f,			0.04f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case WHITE_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.05f,        0.05f,			0.05f );
            setDiffuse  (program,cur_glFunctions, 0.5f,			0.5f,			0.5f );
            setSpecular (program,cur_glFunctions, 0.7f,			0.7f,			0.7f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case YELLOW_RUBBER:
            setAmbient  (program,cur_glFunctions, 0.05f,        0.05f,			0.0f );
            setDiffuse  (program,cur_glFunctions, 0.5f,			0.5f,			0.4f );
            setSpecular (program,cur_glFunctions, 0.7f,			0.7f,			0.04f );
            setShininess(program,cur_glFunctions, 0.078125f );
            break;
        case PEWETER:
            setAmbient  (program,cur_glFunctions, 0.10588f, 0.05824f, 0.113725f );
            setDiffuse  (program,cur_glFunctions, 0.427451f, 0.470588f, 0.541176f );
            setSpecular (program,cur_glFunctions, 0.3333f, 0.3333f,	0.521569f );
            setShininess(program,cur_glFunctions, 51.2f / 128.f );
            break;
        default:
            assert( !"Invalid Standard-Material Index!" );
            break;
        }
    }
}


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
    if(error) exit(1);
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
