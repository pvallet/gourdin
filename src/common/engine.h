#pragma once

#include "opengl.h"

#include <set>
#include <unordered_set>
#include <vector>

#include "antilope.h"
#include "lion.h"
#include "tree.h"
#include "igElementDisplay.h"

#include "texturedRectangle.h"

#include "chunk.h"
#include "contentGenerator.h"
#include "map.h"
#include "ocean.h"
#include "skybox.h"
#include "terrainGeometry.h"

#include "reliefGenerator.h"

#include "terrainTexManager.h"

#include "frameBufferObject.h"
#include "shader.h"

enum ChunkStatus {NOT_GENERATED, EDGE, NOT_VISIBLE, VISIBLE};

#ifndef NDEBUG
	class TestHandler;
#endif

class Engine {

#ifndef NDEBUG
	friend TestHandler;
#endif

public:
	Engine();

	void resetCamera();
  void init();

	void update(int msElapsed);
	void renderToFBO() const;
	void moveSelection(glm::ivec2 screenTarget);
	void moveCamera(glm::vec2 newAimedPos);
	void addLion(glm::ivec2 screenTarget, float minDistToAntilopes = 0);
	std::vector<Controllable*> genTribe(glm::vec2 pos);
	void deleteElements(const std::vector<igMovingElement*>& elementsToDelete);

	inline void switchWireframe() {_wireframe = !_wireframe;}

	glm::vec3 getNormalOnCameraPointedPos() const;

	inline const std::set<Controllable*>& getControllableElements() {return _controllableElements;}
	inline const std::set<Controllable*>& getDeadControllableElements() {return _deadControllableElements;}
  inline const std::vector<std::vector<ChunkStatus> >& getChunkStatus() const {return _chunkStatus;}
	inline const Texture* getColorBuffer() const {return _globalFBO.getColorBuffer();}

	glm::vec2 get2DCoord(glm::ivec2 screenTarget);

private:
  // When the coordinates are (size_t x, size_t y), they are coordinates of the chunk
	glm::ivec2 neighbour      (size_t x, size_t y, size_t index) const;
  void generateNeighbourChunks(size_t x, size_t y);
	void generateChunk          (size_t x, size_t y);
  void appendNewElements(std::vector<igMovingElement*> elems);
	// The parameter contains a list of the moving elements in the chunk (x,y) in index x * NB_CHUNKS + y
  static void updateMovingElementsStates(const std::vector<std::list<igMovingElement*> >& sortedElements);
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

	std::vector<std::vector<std::unique_ptr<Chunk> > > _terrain;

  std::vector<std::vector<ChunkStatus> > _chunkStatus;

	Shader _depthInColorBufferShader;
	FrameBufferObject _globalFBO;
	FrameBufferObject _depthInColorBufferFBO;
	std::unique_ptr<TexturedRectangle> _depthTexturedRectangle;
};
