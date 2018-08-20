#include "../include/BloomRender.h"
#include "../include/GeometryFactory.h"

BloomRender::BloomRender(Window *_window, bool msaa, const char *argv[], unsigned int _iter) :
        iter(_iter), window(_window), msaa(msaa) {
    quad = GeometryFactory::Geometry_create_quad();
    screenShader = new Shader(argv, "screen_trait.vs.glsl", "screen_trait.fs.glsl");
    screenFinal = new Shader(argv, "screen_trait.vs.glsl", "screen_final.fs.glsl");
    init();
}

BloomRender::~BloomRender() {
}

void BloomRender::initBlur() {
    glGenFramebuffers(2, fboblur);
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, fboblur[i]);
        glGenTextures(1, &texblur[i]);
        glBindTexture(GL_TEXTURE_2D, texblur[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, colors[0], GL_TEXTURE_2D, texblur[i], 0);
    }
    GLuint rboblur;
    glGenRenderbuffers(1, &rboblur);
    glBindRenderbuffer(GL_RENDERBUFFER, rboblur);
    glDrawBuffers(1, colors);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRender::genLinearTexture(GLuint tex, GLint format, GLuint i, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, colors[i], GL_TEXTURE_2D, tex, 0);
}

void BloomRender::genMSLinearTexture(GLuint tex, GLint format, GLuint i, int width, int height) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, width, height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, colors[i], GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
}

void BloomRender::init() {
    glGenFramebuffers(1, &msfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
    glGenTextures(2, mstex);
    genMSLinearTexture(mstex[0], GL_RGB, 0, window->getWidth(), window->getHeight());
    genMSLinearTexture(mstex[1], GL_RGB16F, 1, window->getWidth(), window->getHeight());
    glGenRenderbuffers(1, &msrbo);
    glBindRenderbuffer(GL_RENDERBUFFER, msrbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, window->getWidth(), window->getHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msrbo);
    glDrawBuffers(2, colors);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(2, tex);
    genLinearTexture(tex[0], GL_RGB, 0, window->getWidth(), window->getHeight());
    genLinearTexture(tex[1], GL_RGB16F, 1, window->getWidth(), window->getHeight());

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->getWidth(), window->getHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glDrawBuffers(2, colors);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    initBlur();
}

void BloomRender::render(std::function<void()> sceneRender) {

    glEnable(GL_MULTISAMPLE);

    if (!window->activeFilled()) {
        sceneRender();
        return;
    }

    if(msaa) {
        glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    sceneRender();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    if(msaa) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        for (unsigned int i = 0; i < 2; ++i) {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            glBlitFramebuffer(0, 0, window->getWidth(), window->getHeight(),
                              0, 0, window->getWidth(), window->getHeight(),
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    quad->enable();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            glBindFramebuffer(GL_FRAMEBUFFER, fboblur[j]);
            glViewport(0, 0, textureWidth, textureHeight);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glDisable(GL_MULTISAMPLE);
            glPolygonMode(GL_FRONT, GL_FILL);
            screenShader->use();
            screenShader->uniformValue("uTexture_lum", 0);
            if (j == 0) {
                screenShader->uniformValue("hori", 1);
            } else {
                screenShader->uniformValue("hori", 0);
            }
            if (i == 0 && j == 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex[1]);
            } else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texblur[1 - j]);
            }
            quad->draw();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glViewport(0, 0, window->getWidth(), window->getHeight());


    screenFinal->use();
    screenFinal->uniformValue("uTexture", 0);
    screenFinal->uniformValue("uTexture_lum", 1);
    quad->enable();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texblur[1]);
    quad->draw();
    quad->disable();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}