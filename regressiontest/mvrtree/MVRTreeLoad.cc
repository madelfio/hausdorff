// Spatial Index Library
//
// Copyright (C) 2003 Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Email:
//    mhadji@gmail.com

// NOTE: Please read README.txt before browsing this code.

#include <cstring>

#include <cstring>

// include library header file.
#include <SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

#define DELETE 0
#define INSERT 1
#define QUERY 2

// example of a Visitor pattern.
// see MVRTreeQuery for a more elaborate example.
class MyVisitor : public IVisitor
{
public:
	void visitNode(const INode& n) {}

	void visitData(const IData& d)
	{
		cout << d.getIdentifier() << endl;
			// the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v) {}
};

int main(int argc, char** argv)
{
	try
	{
		if (argc != 5)
		{
			cerr << "Usage: " << argv[0] << " input_file tree_file capacity query_type [intersection | 10NN]." << endl;
			return -1;
		}

		uint32_t queryType = 0;

		if (strcmp(argv[4], "intersection") == 0) queryType = 0;
		else if (strcmp(argv[4], "10NN") == 0) queryType = 1;
		else
		{
			cerr << "Unknown query type." << endl;
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open data file " << argv[1] << "." << endl;
			return -1;
		}

		// Create a new storage manager with the provided base name and a 4K page size.
		string baseName = argv[2];
		IStorageManager* diskfile = StorageManager::createNewDiskStorageManager(baseName, 4096);
		//StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
			// applies a main memory random buffer on top of the persistent storage manager
			// (LRU buffer, etc can be created the same way).

		// Create a new, empty, MVRTree with dimensionality 2, minimum load 70%, using "file" as
		// the StorageManager and the RSTAR splitting policy.
		id_type indexIdentifier;
		ISpatialIndex* tree = MVRTree::createNewMVRTree(*diskfile, 0.7, atoi(argv[3]), atoi(argv[3]), 2, SpatialIndex::MVRTree::RV_RSTAR, indexIdentifier);

		size_t count = 0;
		id_type id;
		uint32_t op;
		double x1, x2, y1, y2, t;
		double plow[2], phigh[2];

		while (fin)
		{
			fin >> t >> op >> id >> x1 >> y1 >> x2 >> y2;
			if (! fin.good()) continue; // skip newlines, etc.

			if (op == INSERT)
			{
				plow[0] = x1; plow[1] = y1;
				phigh[0] = x2; phigh[1] = y2;
				TimeRegion r = TimeRegion(plow, phigh, t, t, 2);

				//ostringstream os;
				//os << r;
				//string data = os.str();
					// associate some data with this region. I will use a string that represents the
					// region itself, as an example.
					// NOTE: It is not necessary to associate any data here. A null pointer can be used. In that
					// case you should store the data externally. The index will provide the data IDs of
					// the answers to any query, which can be used to access the actual data from the external
					// storage (e.g. a hash table or a database table, etc.).
					// Storing the data in the index is convinient and in case a clustered storage manager is
					// provided (one that stores any node in consecutive pages) performance will improve substantially,
					// since disk accesses will be mostly sequential. On the other hand, the index will need to
					// manipulate the data, resulting in larger overhead. If you use a main memory storage manager,
					// storing the data externally is highly recommended (clustering has no effect).
					// A clustered storage manager is NOT provided yet.
					// Also you will have to take care of converting you data to and from binary format, since only
					// array of bytes can be inserted in the index (see MVRTree::Node::load and MVRTree::Node::store for
					// an example of how to do that).

				//tree->insertData(data.size() + 1, reinterpret_cast<const byte*>(data.c_str()), r, id);

				tree->insertData(0, 0, r, id);
					// example of passing zero size and a null pointer as the associated data.
			}
			else if (op == DELETE)
			{
				plow[0] = x1; plow[1] = y1;
				phigh[0] = x2; phigh[1] = y2;
				TimeRegion r = TimeRegion(plow, phigh, t, t, 2);

				if (tree->deleteData(r, id) == false)
				{
					cerr << "******ERROR******" << endl;
					cerr << "Cannot delete id: " << id << " , count: " << count << endl;
					return -1;
				}
			}
			else if (op == QUERY)
			{
				size_t qt1, qt2;
				fin >> qt1 >> qt2;
				if (! fin.good()) continue;

				plow[0] = x1; plow[1] = y1;
				phigh[0] = x2; phigh[1] = y2;

				MyVisitor vis;

				if (queryType == 0)
				{
					TimeRegion r = TimeRegion(plow, phigh, qt1, qt2, 2);

					tree->intersectsWithQuery(r, vis);
						// this will find all data that intersect with the query range.
				}
				else
				{
					//Point p = Point(plow, 2);
					//tree->nearestNeighborQuery(10, p, vis);
						// this will find the 10 nearest neighbors.
				}
			}

			if ((count % 1000) == 0)
				cerr << count << endl;

			count++;
		}

		cerr << "Operations: " << count << endl;
		cerr << *tree;
		//cerr << "Buffer hits: " << file->getHits() << endl;
		cerr << "Index ID: " << indexIdentifier << endl;

		bool ret = tree->isIndexValid();
		if (ret == false) cerr << "ERROR: Structure is invalid!" << endl;
		else cerr << "The stucture seems O.K." << endl;

		delete tree;
		//delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail trying to write the dirty entries).
	}
	catch (Tools::Exception& e)
	{
		cerr << "******ERROR******" << endl;
		std::string s = e.what();
		cerr << s << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "******ERROR******" << endl;
		cerr << "other exception" << endl;
		return -1;
	}

	return 0;
}
