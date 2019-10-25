/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2019 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/

#include "mongochemtreemodel.h"

namespace Avogadro {
namespace QtPlugins {

class MongoChemTreeModel::TreeItem
{
public:
  explicit TreeItem(const QVariantMap& data = QVariantMap(),
                    TreeItem* parent = nullptr)
    : m_data(data), m_parent(parent)
  {}
  ~TreeItem() { qDeleteAll(m_children); }

  TreeItem* parent() { return m_parent; }
  void setParent(TreeItem* parent) { m_parent = parent; }
  TreeItem* child(int index) { return m_children.value(index, nullptr); }
  int childCount() const { return m_children.count(); }
  void clearChildren()
  {
    qDeleteAll(m_children);
    m_children.clear();
  }
  void setData(const QVariantMap& data) { m_data = data; }
  const QVariantMap& data() { return m_data; }

  void appendChild(const QVariantMap& data)
  {
    m_children.append(new TreeItem(data, this));
  }

  bool removeChild(int pos)
  {
    if (pos < 0 || pos >= m_children.size())
      return false;

    delete m_children.takeAt(pos);
    return true;
  }

  int row()
  {
    if (!m_parent)
      return -1;

    return m_parent->m_children.indexOf(this);
  }

private:
  QList<TreeItem*> m_children;
  QVariantMap m_data;
  TreeItem* m_parent = nullptr;
};

MongoChemTreeModel::MongoChemTreeModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new TreeItem)
{}

MongoChemTreeModel::~MongoChemTreeModel() = default;

int MongoChemTreeModel::rowCount(const QModelIndex& id) const
{
  Q_UNUSED(id)
  return m_rootItem->childCount();
}

int MongoChemTreeModel::columnCount(const QModelIndex& /*parent*/) const
{
  return 3;
}

QVariant MongoChemTreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  switch (role) {
    case Qt::DisplayRole: {
      auto item = getItem(index);
      const auto& mol = item->data();
      int column = index.column();
      switch (column) {
        case 0:
          return mol.value("properties").toMap().value("formula");
        case 1:
          return mol.value("smiles");
        case 2:
          return mol.value("inchikey");
        default:
          return QVariant();
      }
    }
  }

  return QVariant();
}

Qt::ItemFlags MongoChemTreeModel::flags(const QModelIndex& index) const
{
  return QAbstractItemModel::flags(index);
}

QVariant MongoChemTreeModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical)
    return section + 1;

  if (orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return tr("Formula");

      case 1:
        return tr("SMILES");

      case 2:
        return tr("InChIKey");
    }
  }

  return QVariant();
}

QModelIndex MongoChemTreeModel::parent(const QModelIndex& child) const
{
  auto item = getItem(child);
  if (!item)
    return QModelIndex();

  auto parentItem = item->parent();
  if (!parentItem)
    return QModelIndex();

  return createIndex(parentItem->row(), child.column(), parentItem);
}

QModelIndex MongoChemTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  auto parentItem = getItem(parent);
  auto childItem = parentItem->child(row);
  if (!childItem)
    return QModelIndex();

  return createIndex(row, column, childItem);
}

QString MongoChemTreeModel::moleculeId(int row)
{
  auto item = m_rootItem->child(row);
  if (!item)
    return "";

  return item->data()["_id"].toString();
}

QString MongoChemTreeModel::moleculeName(int row)
{
  auto item = m_rootItem->child(row);
  if (!item)
    return "";

  auto name = item->data()["name"].toString();

  // If there is no name, use the formula instead
  if (name.isEmpty())
    name = item->data()["properties"].toMap()["formula"].toString();

  return name;
}

void MongoChemTreeModel::addMolecule(const QVariantMap& mol)
{
  beginInsertRows(QModelIndex(), m_rootItem->childCount(),
                  m_rootItem->childCount());
  m_rootItem->appendChild(mol);
  endInsertRows();
}

void MongoChemTreeModel::deleteMolecule(const QModelIndex& index)
{
  if (!index.isValid())
    return;

  int row = index.row();
  if (row < m_rootItem->childCount()) {
    beginRemoveRows(QModelIndex(), row, row);
    m_rootItem->removeChild(row);
    endRemoveRows();
  }
}

void MongoChemTreeModel::clear()
{
  bool noChildren = m_rootItem->childCount() == 0;
  int lastRow = noChildren ? 0 : m_rootItem->childCount() - 1;
  beginRemoveRows(QModelIndex(), 0, lastRow);
  m_rootItem->clearChildren();
  endRemoveRows();
}

MongoChemTreeModel::TreeItem* MongoChemTreeModel::getItem(
  const QModelIndex& index) const
{
  if (index.isValid()) {
    auto item = static_cast<TreeItem*>(index.internalPointer());
    if (item)
      return item;
  }

  return m_rootItem.data();
}

} // namespace QtPlugins
} // namespace Avogadro
