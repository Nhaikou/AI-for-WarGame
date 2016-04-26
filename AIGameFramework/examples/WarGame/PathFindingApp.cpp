#include "PathFindingApp.h"
#include <Input.h>
#include <ElapsedTimer.h>
#include <StreamTexture.h>
#include <Text.h>
#include "OpenList.h"
#include "ClosedList.h"
#include "SearchLevel.h"
#include "SearchNode.h"

//namespace
//{
//	void setPathColor(yam2d::StreamTexture* t, int x, int y)
//	{
//		t->getPixel(x, y)[0] = 0xff;
//		t->getPixel(x, y)[1] = 0x00;
//		t->getPixel(x, y)[2] = 0xff;
//	}
//
//	bool isRed(unsigned char* p)
//	{
//		return p[0] > 200;
//	}
//
//	bool isGreen(unsigned char* p)
//	{
//		return p[1] > 200;
//	}
//
//	bool isBlue(unsigned char* p)
//	{
//		return p[2] > 200;
//	}
//}

PathFindingApp::PathFindingApp()
	: m_batch()
	, m_spriteStartCase()
	, m_spritePathFound()
{
	m_batch = new yam2d::SpriteBatchGroup();

	// Load font texture. Made with font creation tool like bitmap font builder.
	m_fontTexture = new yam2d::Texture("Fixedsys_24_Bold.png");
	yam2d::SpriteSheet* font = yam2d::SpriteSheet::autoFindFontFromTexture(m_fontTexture, "Fixedsys_24_Bold.dat");
	m_text = new yam2d::Text(0, font);
	m_text->setText("Path finding example");

	m_spriteStartCase = new yam2d::Sprite(0);
	m_spritePathFound = new yam2d::Sprite(0);

	m_searchCompleted = false;
	m_searchTimer = 0.0f;

}



PathFindingApp::~PathFindingApp()
{
}


bool PathFindingApp::update(yam2d::ESContext* ctx, float deltaTime)
{
	if (!m_appRunning)
		return false;

	if (deltaTime > 0.1f)
		deltaTime = 0.1f;

#if defined(_WIN32)
	if (isKeyReleased(yam2d::KEY_SPACE))
	{
		quit();
	}

	// Restart search if r pressed
	if (isKeyReleased(yam2d::KEY_R))
	{
		m_textureStartCase = 0;
		m_texturePathFound = 0;
	}
#endif

	if (m_textureStartCase == 0)
	{
		// Delete old and load new
		m_texturePathFound = 0;
		const char* const inFileName = "testMap.png";

		char buf[100];
		sprintf_s(buf, "Start finding path from input image: \"%s\"", inFileName);
		yam2d::esLogMessage(buf);
		m_text->setText(buf);
		m_textureStartCase = new yam2d::Texture(inFileName, true);

		// Copy input data to map.
		m_searchTimer = 0.0f;
		int width = m_textureStartCase->getWidth();
		int height = m_textureStartCase->getHeight();
		int bpp = m_textureStartCase->getBytesPerPixel();
		yam2d::Ref<yam2d::StreamTexture> newTexture = new yam2d::StreamTexture();
		newTexture->setData(m_textureStartCase->getData(), width, height, bpp);
		m_texturePathFound = newTexture;
		m_searchCompleted = false;
	}

	if (!m_searchCompleted)
	{
		// Find start and end
		int startX, startY, endX, endY;
		startX = startY = endX = endY = -1;
		for (int y = 0; y < m_textureStartCase->getHeight(); ++y)
		{
			for (int x = 0; x < m_textureStartCase->getWidth(); ++x)
			{
				unsigned char* p = m_textureStartCase->getPixel(x, y);
				if (isRed(p))
				{
					// Red pixel
					startX = x;
					startY = y;
				}
				else if (isGreen(p))
				{
					// Green pixel
				}
				else if (isBlue(p))
				{
					// Blue pixel
					endX = x;
					endY = y;
				}
			}
		}

		// Update path find!! Set m_searchCompleted to true, when path found, so the texture data is updated.
		if (startX >= 0 && startY >= 0 && endX >= 0 && endY >= 0)
		{
			yam2d::ElapsedTimer timer;
			timer.reset();
			m_searchCompleted = doPathfinding(startX, startY, endX, endY);
			m_searchTimer += timer.getTime();
			// 	Update new data to the GPU
			m_texturePathFound->updateData();
		}
		else
		{
			assert(0);
		}

		if (m_searchCompleted)
		{
			char buf[100];
			sprintf_s(buf, "Path find done. Time spent %.3f seconds", m_searchTimer);
			yam2d::esLogMessage("%s\n", buf);
			m_text->setText(buf);
		}
	} // if (!m_searchCompleted)

	// Clear sprite before add new dynamic sprites.
	m_batch->clear();

	// Add sprites. 
	m_batch->addSprite(m_textureStartCase, m_spriteStartCase, yam2d::vec2(-256.0f - 20.0f, 0.0f), 0.0f, yam2d::vec2(512.0f, 512.0f));
	m_batch->addSprite(m_texturePathFound, m_spritePathFound, yam2d::vec2(256.0f + 20.0f, 0.0f), 0.0f, yam2d::vec2(512.0f, 512.0f));

	// Add text to position -400,300
	m_batch->addText(m_fontTexture, m_text, yam2d::vec2(0, ((float)ctx->height) / 2.0f - 20.0f), 0.0f);

	return true;
}

void PathFindingApp::render(yam2d::ESContext *ctx)
{
	// Set OpenGL clear color
	glViewport(0, 0, ctx->width, ctx->height);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set projection to identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Calculate half screen size
	float left = -0.5f*ctx->width;
	float right = 0.5f*ctx->width;
	float bottom = -0.5f*ctx->height;
	float top = 0.5f*ctx->height;

	// Set OpenGL orthogonal projection for screen size <esContext->width,esContext->height>
	glOrthof(float(int(left)), float(int(right)), float(int(bottom)), float(int(top)), -1.0, 1.0f);

	// Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Enable alpha blending (transparency)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set model view matrix to identity
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	m_batch->render();
}


bool PathFindingApp::doPathfinding(int startX, int startY, int endX, int endY)
{

	// Set color for start and end pos to path color
	setPathColor(m_texturePathFound, startX, startY);
	setPathColor(m_texturePathFound, endX, endY);

	bool done = true;
	// Some variables for path finding
	OpenList openList;
	ClosedList closedList;
	SearchLevel searchLevel(m_textureStartCase);
	SearchNode *result = 0;

	// Start A* search:
	// Add start node to open list.
	Position startPosition = Position(startX, startY);
	Position endPosition = Position(endX, endY);
	SearchNode *newNode = new SearchNode(startPosition, searchLevel.getH(startPosition, endPosition), 0, 0);
	openList.insertToOpenList(newNode);

	// 1. Get the square on the open list which has the lowest score. Let's call this square S (or prey)
	while (!openList.isEmpty())
	{
		SearchNode *prev = openList.removeSmallestFFromOpenList();
		if (prev->pos == endPosition)
		{
			// Goal found!
			result = prev;
			break;
		}
		else
		{
			// 2. Remove S from the open list and add S to the closed list.
			closedList.addToClosedList(prev);

			// 3. for each square T in S's walkable adjacent tiles:
			std::vector<Position> adjacentNodes = searchLevel.getAdjacentNodes(prev->pos.first, prev->pos.second);
			for (size_t i = 0; i < adjacentNodes.size(); ++i)
			{
				// If T is in the closed list : Ignore it.
				if (closedList.isInClosedList(adjacentNodes[i]))
				{
					continue;
				}

				SearchNode *n = openList.findFromOpenList(adjacentNodes[i]);
				if (n == 0)
				{
					// If T is not in the open list : Add it and compute its score
					SearchNode * newNode = new SearchNode(adjacentNodes[i],
						searchLevel.getH(adjacentNodes[i], endPosition),
						searchLevel.getG(prev, adjacentNodes[i]), prev);
					openList.insertToOpenList(newNode);
				}
				else
				{
					// If T is already in the open list : Check if the F score is lower
					// when we use the current generated path to get there. If it is update
					// it's score and update its parent as well.
					SearchNode *newNode = new SearchNode(adjacentNodes[i],
						searchLevel.getH(adjacentNodes[i], endPosition),
						searchLevel.getG(prev, adjacentNodes[i]), prev);
					if (n->distance() < newNode->distance())
					{
						n->resetPrev(newNode->prevNode, searchLevel.getG(newNode->prevNode, n->pos));
					}
				}
			}
		}
	}
	if (result == 0)
	{
		printf("Path not found!!!\n");
		return true;
	}

	while (result != 0)
	{
		setPathColor(m_texturePathFound, result->pos.first, result->pos.second);
		result = result->prevNode;
	}

	return true;

	// TODO: Remove that search end and delay hack as seen below..
	//static int i = 0;
	//i = ((i+1)%10); // 10*100ms = ~500 ms of total
	//Sleep(100);
	//return i==0;
}

