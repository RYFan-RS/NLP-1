#ifndef __ROBOSAY__SORT_MANAGER__H__
#define __ROBOSAY__SORT_MANAGER__H__

#include <algorithm>
#include <functional>
#include <vector>
namespace robosay{
namespace  base{
	template<class ElemType, class WeightType = int>
	class CSortManager
	{
	public:
		typedef struct NodeElem
		{
		public:	
			NodeElem(const WeightType& iWeight, const ElemType& elem)
				:m_iWeight(iWeight),m_elem(elem)
			{}
		
			bool operator > (const NodeElem& right)const
			{
				return m_iWeight > right.m_iWeight;
			}
			
			bool operator < (const NodeElem& right)const
			{
				return m_iWeight < right.m_iWeight;
			}

		public:
			const WeightType& GetWeight()const{ return m_iWeight; }
			const ElemType& GetElem()const { return m_elem; }

		private:	
			WeightType m_iWeight;
			ElemType m_elem;
			
			friend class CSortManager<ElemType,WeightType>;
		}NodeElem;
		
	public:
		void Add(const WeightType& iWeight, const ElemType& elem)
		{
			m_elems.push_back(NodeElem(iWeight,elem));
		}
		
		void Sort(bool bIsDescendingSortByWeight=true)
		{
			if (bIsDescendingSortByWeight)
			{
				//descending sort by weight	
				return std::sort(m_elems.begin(),m_elems.end(), std::greater<NodeElem>());
			}
			
			//sascending sort by weight	
			std::sort(m_elems.begin(),m_elems.end(), std::less<NodeElem>());
		}
		
		const std::vector<NodeElem>& GetAllElems()const
		{
			return m_elems;
		}
		
		//BackCallFunc ¿‡À∆£∫void (*)(int, const ElemType&)
		template<class BackCallFunc>
		void ForEach(BackCallFunc backCallFunc)const
		{
			typename std::vector<NodeElem>::const_iterator iter = m_elems.begin();
			while (iter != m_elems.end())
			{
				(void)backCallFunc(iter->m_iWeight,iter->m_elem);
				++iter;
			}
		}
		
	private:
		std::vector<NodeElem> m_elems;
	};
}
}
#endif