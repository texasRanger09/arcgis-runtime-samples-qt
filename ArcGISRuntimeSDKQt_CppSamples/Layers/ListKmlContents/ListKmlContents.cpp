// [WriteFile Name=ListKmlContents, Category=Layers]
// [Legal]
// Copyright 2020 Esri.

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

#include "ListKmlContents.h"

#include "ArcGISTiledElevationSource.h"
#include "KmlContainer.h"
#include "KmlDataset.h"
#include "KmlLayer.h"
#include "KmlNetworkLink.h"
#include "KmlNode.h"
#include "KmlNodeListModel.h"
#include "Scene.h"
#include "SceneQuickView.h"

#include <QDir>

using namespace Esri::ArcGISRuntime;

// helper method to get cross platform data path
namespace
{
QString defaultDataPath()
{
  QString dataPath;

#ifdef Q_OS_ANDROID
  dataPath = "/sdcard";
#elif defined Q_OS_IOS
  dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
  dataPath = QDir::homePath();
#endif

  return dataPath;
}

int myCounter = 0;
} // namespace

ListKmlContents::ListKmlContents(QObject* parent /* = nullptr */):
  QObject(parent),
  m_scene(new Scene(Basemap::imageryWithLabels(this), this))
{
  // create a new elevation source from Terrain3D REST service
  ArcGISTiledElevationSource* elevationSource = new ArcGISTiledElevationSource(
        QUrl("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"), this);

  // add the elevation source to the scene to display elevation
  m_scene->baseSurface()->elevationSources()->append(elevationSource);

  const QString filepath = defaultDataPath() + "/ArcGIS/Runtime/Data/kml/esri_test_data.kmz";

  // create new KML layer
  m_kmlDataset = new KmlDataset(filepath, this);
  KmlLayer* kmlLayer = new KmlLayer(m_kmlDataset, this);
  m_scene->operationalLayers()->append(kmlLayer);

  connect(m_kmlDataset, &KmlDataset::doneLoading, this, [this](Error loadError)
  {
    if (!loadError.isEmpty())
    {
      qDebug() << loadError.message() << loadError.additionalMessage();
      return;
    }

    // recursively build tree to display KML contents
    for (int i = 0; i < m_kmlDataset->rootNodes().size(); ++i)
    {
      m_levelNodeNames << m_kmlDataset->rootNodes()[i]->name();
      m_kmlNodesList << m_kmlDataset->rootNodes()[i];

      buildTree(m_kmlDataset->rootNodes()[i]);
    }

    emit nodeNamesChanged();

    // if at top node, then display children
    if (!m_kmlNodesList.isEmpty() && m_kmlNodesList[0]->parentNode() == nullptr)
    {
      displayChildren(m_kmlNodesList[0]);
    }
  });
}

void ListKmlContents::displayChildren(KmlNode *parentNode)
{
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(parentNode))
  {
    m_levelNodeNames.clear();

    // for current level, get names of child nodes
    for (KmlNode* node: *(container->childNodesListModel()))
    {
      m_levelNodeNames << node->name();
      m_kmlNodesList << node;
    }

    emit levelNodeNamesChanged();
  }
}

void ListKmlContents::displayPreviousLevel()
{
  KmlNode* parentNode = m_currentNode->parentNode();
  KmlNode* grandparentNode = parentNode->parentNode();
//  if (grandparentNode == nullptr || grandparentNode->name() == "")
//  {
//    displayChildren(parentNode);
//    m_isTopLevel = true;
//    emit isTopLevelChanged();
//    qDebug() << "nullptr, " << m_currentNode->name();
//  }
//  // if parent node is nullptr, then at top of tree
//  else
//  {
//    qDebug() << "Grandparent node: " << grandparentNode->name();
//    displayChildren(grandparentNode);
//    m_currentNode = grandparentNode;
//    qDebug() << "(displayPreviousLevel) current node: " << m_currentNode->name();
//  }

  if (grandparentNode != nullptr)
  {
    displayChildren(grandparentNode);
    m_currentNode = grandparentNode;
  }
  // if parent node is nullptr, then at top of tree
  else
  {
    displayChildren(parentNode);
    m_isTopLevel = true;
    emit isTopLevelChanged();
  }

  if (m_currentNode->name() == "")
  {
    m_isTopLevel = true;
    emit isTopLevelChanged();
  }
}

void ListKmlContents::nodeSelected(const QString nodeName)
{
  for (KmlNode* node : m_kmlNodesList)
  {
    if (nodeName == node->name())
    {
      // update current node
      m_currentNode = node;
      m_isTopLevel = false;
      emit isTopLevelChanged();

      // set the scene view viewpoint to the extent of the selected node
      Envelope nodeExtent = node->extent();
      if (nodeExtent.isValid() && !nodeExtent.isEmpty())
      {
        m_sceneView->setViewpoint(Viewpoint(nodeExtent));
      }

      // show the children of the node
      displayChildren(node);

      break;
    }
  }
}

void ListKmlContents::buildTree(KmlNode* parentNode)
{
  if (KmlContainer* container = dynamic_cast<KmlContainer*>(parentNode))
  {
    ++myCounter;
    KmlNodeListModel* childNodes = container->childNodesListModel();

    for (KmlNode* node : *childNodes)
    {
      // some nodes have default visibility set to false
      node->setVisible(true);

      // add spaces to show hierarchy
      const QString str = node->name().rightJustified(node->name().length() + myCounter*2, ' ');
      m_nodeNames << str;
      emit nodeNamesChanged();


      buildTree(node);
    }
    --myCounter;
  }

  if (KmlNetworkLink* networkLink = dynamic_cast<KmlNetworkLink*>(parentNode))
  {
    qDebug("NETWORK LINK");
  }
}

ListKmlContents::~ListKmlContents() = default;

void ListKmlContents::init()
{
  // Register classes for QML
  qmlRegisterType<SceneQuickView>("Esri.Samples", 1, 0, "SceneView");
  qmlRegisterType<ListKmlContents>("Esri.Samples", 1, 0, "ListKmlContentsSample");
}

SceneQuickView* ListKmlContents::sceneView() const
{
  return m_sceneView;
}

// Set the view (created in QML)
void ListKmlContents::setSceneView(SceneQuickView* sceneView)
{
  if (!sceneView || sceneView == m_sceneView)
    return;

  m_sceneView = sceneView;
  m_sceneView->setArcGISScene(m_scene);

  emit sceneViewChanged();
}

