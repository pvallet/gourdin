#pragma once

#include "opengl.h"

#include <set>
#include <unordered_set>
#include <vector>

#include "antilope.h"
#include "lion.h"
#include "tree.h"
#include "igElementDisplay.h"

#include "loadingScreen.h"
#include "texturedRectangle.h"

#include "chunk.h"
#include "chunkSubdivider.h"
#include "contentGenerator.h"
#include "map.h"
#include "ocean.h"
#include "skybox.h"
#include "terrainGeometry.h"

#include "reliefGenerator.h"

#include "terrainTexManager.h"

#include "frameBufferObject.h"
#include "shader.h"

#ifndef NDEBUG
	class TestHandler;
#endif

class Engine {

#ifndef NDEBUG
	friend TestHandler;
#endif

public:
	Engine();
	~Engine();
	Engine        (Engine const&) = delete;
	void operator=(Engine const&) = delete;

  void init(LoadingScreen& loadingScreen);

	void update(int msElapsed);
	void renderToFBO() const;
	void moveSelection(glm::ivec2 screenTarget);
	void addLion(glm::ivec2 screenTarget, float minDistToAntilopes = 0);
	std::vector<Controllable*> genTribe(glm::ivec2 screenTarget);
	void deleteElements(const std::vector<igMovingElement*>& elementsToDelete);

	inline void switchWireframe() {_wireframe = !_wireframe;}
	inline void waitForTasksToFinish() {_chunkSubdivider.waitForTasksToFinish();}

	inline const std::set<Controllable*>& getControllableElements() {return _controllableElements;}
	inline const std::set<Controllable*>& getDeadControllableElements() {return _deadControllableElements;}
	inline const Texture* getColorBuffer() const {return _globalFBO.getColorBuffer();}

	inline bool isChunkVisible(size_t x, size_t y) const {return _terrain[x*NB_CHUNKS + y]->isVisible();}

	glm::vec2 get2DCoord(glm::ivec2 screenTarget);
	glm::vec3 getNormalOnCameraPointedPos() const;
	float getHeight(glm::vec2 pos) const;

private:
  void appendNewElements(std::vector<igMovingElement*> elems);
	// The parameter contains a list of the moving elements in the chunk (x,y) in index x * NB_CHUNKS + y
  void updateMovingElementsStates();
	void updateCulling();
	void compute2DCorners();

	bool _wireframe;

  std::list<std::unique_ptr<igMovingElement> > _igMovingElements;

  // Raw pointers because the ownership is in _igMovingElements
	// Static elements are stored in chunks
  std::set<Controllable*> _controllableElements;
	std::set<Controllable*> _deadControllableElements;

	igElementDisplay _igElementDisplay;
  ContentGenerator _contentGenerator;

  Map _map;
  Ocean _ocean;
	Skybox _skybox;
  TerrainGeometry _terrainGeometry;

	MapInfoExtractor _mapInfoExtractor;
	ReliefGenerator  _reliefGenerator;

	TerrainTexManager _terrainTexManager;
	Shader _terrainShader;
  Shader _igEShader;
	Shader _skyboxShader;

	ChunkSubdivider _chunkSubdivider;
	std::vector<std::unique_ptr<Chunk> > _terrain;

	Shader _depthInColorBufferShader;
	FrameBufferObject _globalFBO;
	FrameBufferObject _depthInColorBufferFBO;
	std::unique_ptr<TexturedRectangle> _depthTexturedRectangle;
};
