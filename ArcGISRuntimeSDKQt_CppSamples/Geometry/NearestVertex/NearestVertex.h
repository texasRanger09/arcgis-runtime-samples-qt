// [WriteFile Name=NearestVertex, Category=Geometry]
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

#ifndef NEARESTVERTEX_H
#define NEARESTVERTEX_H

#include "GeometryEngine.h"

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class ProximityResult;
}
}

#include <QObject>

class NearestVertex : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(Esri::ArcGISRuntime::ProximityResult vertexResult READ vertexDistance NOTIFY vertexDistanceCalculated)
  Q_PROPERTY(Esri::ArcGISRuntime::ProximityResult coordinateDistance READ coordinateDistance NOTIFY coordinateDistanceCalculated )

public:
  explicit NearestVertex(QObject* parent = nullptr);
  ~NearestVertex();

  static void init();

signals:
  void mapViewChanged();
  void vertexDistanceCalculated(Esri::ArcGISRuntime::ProximityResult vertexDistance);
  void coordinateDistanceCalculated();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  Esri::ArcGISRuntime::ProximityResult vertexDistance() const;
  Esri::ArcGISRuntime::ProximityResult coordinateDistance() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::ProximityResult m_vertexResult;
  Esri::ArcGISRuntime::ProximityResult m_coordinateDistance;
};

#endif // NEARESTVERTEX_H