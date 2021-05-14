using System.Collections.ObjectModel;
using MHWItemBoxTracker.Utils;

namespace MHWItemBoxTracker.Model {
  public class InventoryModel : NotifyPropertyChanged {
    private bool inVillage = false;
    public bool inQuest = false;
    public ObservableCollection<InventoryItemModel> items = new();

    public bool InVillage {
      get => inVillage;
      set => SetField(ref inVillage, value);
    }

    public bool InQuest {
      get => inQuest;
      set => SetField(ref inQuest, value);
    }

    public ObservableCollection<InventoryItemModel> Items {
      get => items;
      set => SetField(ref items, value);
    }
  }
}
