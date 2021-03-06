// [WriteFile Name=Surface_Placement, Category=Scenes]
// [Legal]
// Copyright 2016 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#ifdef PCH_BUILD
#include "pch.hpp"
#endif // PCH_BUILD

#include "Surface_Placement.h"

#include "ArcGISTiledElevationSource.h"
#include "Scene.h"
#include "SceneQuickView.h"
#include "Viewpoint.h"
#include "Point.h"
#include "SpatialReference.h"
#include "Camera.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "SimpleMarkerSymbol.h"
#include "TextSymbol.h"
#include "Point.h"

using namespace Esri::ArcGISRuntime;

Surface_Placement::Surface_Placement(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

Surface_Placement::~Surface_Placement() = default;

void Surface_Placement::init()
{
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<Surface_Placement>("Esri.Samples", 1, 0, "SurfacePlacementSample");
}

void Surface_Placement::componentComplete()
{
  QQuickItem::componentComplete();

  //! [Create Scene API snippet]
  // Create a scene with the imagery basemap and world elevation surface
  m_sceneView = findChild<SceneQuickView*>("sceneView");
  Scene* scene = new Scene(Basemap::imagery(this), this);
  Surface* surface = new Surface(this);
  surface->elevationSources()->append(new ArcGISTiledElevationSource(QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this));
  scene->setBaseSurface(surface);
  //! [Create Scene API snippet]

  // set an initial viewpoint
  Camera camera(53.05, -4.01, 1115, 299, 88, 0);
  Viewpoint viewpoint(53.05, -4.01, 1115, camera);
  scene->setInitialViewpoint(viewpoint);

  // set the scene to the scene view
  m_sceneView->setArcGISScene(scene);

  // add graphics overlays and set surface placement
  addGraphicsOverlays();

  // add graphics
  addGraphics();
}

void Surface_Placement::addGraphicsOverlays()
{
  // Graphics overlay with draped billboarded surface placement
  m_drapedBillboardedOverlay = new GraphicsOverlay(this);
  m_drapedBillboardedOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::DrapedBillboarded));
  m_sceneView->graphicsOverlays()->append(m_drapedBillboardedOverlay);

  // Graphics overlay with draped flat surface placement
  m_drapedFlatOverlay = new GraphicsOverlay(this);
  m_drapedFlatOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::DrapedFlat));
  m_drapedFlatOverlay->setVisible(false);
  m_sceneView->graphicsOverlays()->append(m_drapedFlatOverlay);

  // Graphics overlay with relative surface placement
  m_relativeOverlay = new GraphicsOverlay(this);
  m_relativeOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Relative));
  m_sceneView->graphicsOverlays()->append(m_relativeOverlay);

  // Graphics overlay with absolute surface placement
  m_absoluteOverlay = new GraphicsOverlay(this);
  m_absoluteOverlay->setSceneProperties(LayerSceneProperties(SurfacePlacement::Absolute));
  m_sceneView->graphicsOverlays()->append(m_absoluteOverlay);
}

void Surface_Placement::addGraphics()
{
  // create point for the graphics with z value of 1000
  Point point(-4.04, 53.06, 1000, SpatialReference::wgs84());

  // create simple marker symbol
  SimpleMarkerSymbol* simpleMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Triangle, QColor("red"), 20, this);

  // create text symbols
  TextSymbol* drapedBillboardedText = new TextSymbol("DRAPED BILLBOARDED", QColor("white"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
  drapedBillboardedText->setOffsetX(20);
  TextSymbol* drapedFlatText = new TextSymbol("DRAPED FLAT", QColor("white"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
  drapedFlatText->setOffsetX(20);
  TextSymbol* relativeText = new TextSymbol("RELATIVE", QColor("white"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
  relativeText->setOffsetX(20);
  TextSymbol* absoluteText = new TextSymbol("ABSOLUTE", QColor("white"), 20, HorizontalAlignment::Left, VerticalAlignment::Middle, this);
  absoluteText->setOffsetX(20);

  // add graphics to each overlay
  // Graphics will be draped on the surface of the scene and will always face the camera.
  m_drapedBillboardedOverlay->graphics()->append(new Graphic(point, simpleMarkerSymbol));
  m_drapedBillboardedOverlay->graphics()->append(new Graphic(point, drapedBillboardedText));

  // Graphics will be draped on the surface of the scene
  m_drapedFlatOverlay->graphics()->append(new Graphic(point, simpleMarkerSymbol));
  m_drapedFlatOverlay->graphics()->append(new Graphic(point, drapedFlatText));

  // Graphics will be placed at z value relative to the surface
  m_relativeOverlay->graphics()->append(new Graphic(point, simpleMarkerSymbol));
  m_relativeOverlay->graphics()->append(new Graphic(point, relativeText));

  // Graphics will be placed at absolute z value
  m_absoluteOverlay->graphics()->append(new Graphic(point, simpleMarkerSymbol));
  m_absoluteOverlay->graphics()->append(new Graphic(point, absoluteText));
}

void Surface_Placement::changeDrapedVisibility()
{
  m_drapedFlatOverlay->setVisible(!m_drapedFlatOverlay->isVisible());
  m_drapedBillboardedOverlay->setVisible(!m_drapedBillboardedOverlay->isVisible());
}
