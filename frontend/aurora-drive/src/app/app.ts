import { Component, computed, signal } from '@angular/core';

type FileKind = 'folder' | 'document' | 'image' | 'video' | 'archive';
type SortBy = 'server' | 'name' | 'modified' | 'size';
type Theme = 'aurora' | 'evernight';
type PlaceholderDialog = 'help' | 'notifications';
type Locale = 'zh-CN' | 'zh-TW' | 'ja-JP' | 'en-US' | 'fr-FR';
type Section = 'files' | 'recent' | 'favorites' | 'settings' | 'trash';

const localeStorageKey = 'aurora-drive.locale';

const supportedLocales: ReadonlyArray<{ code: Locale; label: string }> = [
  { code: 'zh-CN', label: '简体中文（中国大陆）' },
  { code: 'zh-TW', label: '繁體中文（台灣）' },
  { code: 'ja-JP', label: '日本語（日本）' },
  { code: 'en-US', label: 'English (US)' },
  { code: 'fr-FR', label: 'Français (France)' },
];

const translations: Record<Locale, Record<string, string>> = {
  'zh-CN': {
    'nav.files': '我的文件', 'nav.recent': '最近使用', 'nav.favorites': '收藏', 'nav.settings': '设置', 'nav.trash': '回收站',
    space: '我的空间', disk: '磁盘空间', personalSpace: '个人空间', search: '搜索文件、文件夹或成员',
    help: '帮助', notifications: '通知', close: '关闭', default: '默认', sort: '排序方式', sortModified: '最近修改', sortName: '名称', sortSize: '文件大小',
    listView: '列表视图', gridView: '网格视图', items: '个项目', updatedToday: '最后更新于今天', favoritesNote: '已收藏的文件', trashNote: '可恢复或永久删除',
    filesDescription: '所有文件都会自动同步到你的设备', favoritesDescription: '已收藏的文件会集中显示在这里', trashDescription: '文件将在删除 30 天后自动永久清除', settingsDescription: '调整 Aurora Drive 的显示偏好',
    newFolder: '新建文件夹', uploadFile: '上传文件', selected: '已选择', download: '下载', move: '移动', share: '共享', favorite: '收藏', unfavorite: '取消收藏', delete: '删除', cancelSelection: '取消选择',
    name: '名称', fileType: '文件类型', modifiedDate: '修改日期', deletedDate: '删除日期', fileSize: '文件大小', selectAll: '选择所有项目',
    restore: '恢复', permanentDelete: '永久删除', emptyTrash: '回收站为空', emptyFavorites: '暂无收藏文件', loadingFiles: '正在读取文件列表', filesUnavailable: '无法连接文件服务', emptyFiles: '此目录暂无文件',
    language: '语言', languageDescription: '选择 Aurora Drive 的显示语言', theme: '主题', themeDescription: '选择界面的明暗外观', aurora: 'Aurora', evernight: 'Evernight', auroraDescription: '明亮冷色', evernightDescription: '深色浅红',
    storageLoading: '正在读取所在分区用量', storageUnavailable: '等待系统存储服务连接', refreshUsage: '刷新用量', switchEvernight: '切换至 Evernight 模式', switchAurora: '切换至 Aurora 模式',
    folderTitle: '新建文件夹', folderDescription: '为你的文件创建一个新位置', folderLabel: '文件夹名称', folderPlaceholder: '例如：项目资料', cancel: '取消', createFolder: '创建文件夹', uploadTitle: '上传文件', uploadDescription: '文件将上传到当前文件夹', dropTitle: '拖动文件到这里上传', dropBrowse: '或点击这里浏览本机文件', localFile: '本地文件', remove: '移除已选文件', helpCenter: '帮助中心', helpDescription: '帮助与支持功能', notificationCenter: '通知中心', notificationDescription: '系统消息与动态', inDevelopment: '功能目前正在开发中', helpSoon: '帮助文档与联系支持将很快提供。', notificationsSoon: '通知消息将在后续版本接入。', ok: '知道了',
  },
  'zh-TW': {
    'nav.files': '我的檔案', 'nav.recent': '最近使用', 'nav.favorites': '收藏', 'nav.settings': '設定', 'nav.trash': '回收桶',
    space: '我的空間', disk: '磁碟空間', personalSpace: '個人空間', search: '搜尋檔案、資料夾或成員',
    help: '說明', notifications: '通知', close: '關閉', default: '預設', sort: '排序方式', sortModified: '最近修改', sortName: '名稱', sortSize: '檔案大小',
    listView: '清單檢視', gridView: '格狀檢視', items: '個項目', updatedToday: '最後更新於今天', favoritesNote: '已收藏的檔案', trashNote: '可還原或永久刪除',
    filesDescription: '所有檔案都會自動同步到你的裝置', favoritesDescription: '已收藏的檔案會集中顯示在這裡', trashDescription: '檔案將在刪除 30 天後自動永久清除', settingsDescription: '調整 Aurora Drive 的顯示偏好',
    newFolder: '新增資料夾', uploadFile: '上傳檔案', selected: '已選擇', download: '下載', move: '移動', share: '分享', favorite: '收藏', unfavorite: '取消收藏', delete: '刪除', cancelSelection: '取消選擇',
    name: '名稱', fileType: '檔案類型', modifiedDate: '修改日期', deletedDate: '刪除日期', fileSize: '檔案大小', selectAll: '選擇所有項目',
    restore: '還原', permanentDelete: '永久刪除', emptyTrash: '回收桶是空的', emptyFavorites: '尚無收藏檔案', loadingFiles: '正在讀取檔案清單', filesUnavailable: '無法連線檔案服務', emptyFiles: '此資料夾尚無檔案',
    language: '語言', languageDescription: '選擇 Aurora Drive 的顯示語言', theme: '主題', themeDescription: '選擇介面的明暗外觀', aurora: 'Aurora', evernight: 'Evernight', auroraDescription: '明亮冷色', evernightDescription: '深色淺紅',
    storageLoading: '正在讀取所在分區用量', storageUnavailable: '等待系統儲存服務連線', refreshUsage: '重新整理用量', switchEvernight: '切換至 Evernight 模式', switchAurora: '切換至 Aurora 模式',
    folderTitle: '新增資料夾', folderDescription: '為你的檔案建立新的位置', folderLabel: '資料夾名稱', folderPlaceholder: '例如：專案資料', cancel: '取消', createFolder: '建立資料夾', uploadTitle: '上傳檔案', uploadDescription: '檔案將上傳至目前資料夾', dropTitle: '拖曳檔案到這裡上傳', dropBrowse: '或點擊這裡瀏覽本機檔案', localFile: '本機檔案', remove: '移除已選檔案', helpCenter: '說明中心', helpDescription: '說明與支援功能', notificationCenter: '通知中心', notificationDescription: '系統訊息與動態', inDevelopment: '功能目前正在開發中', helpSoon: '說明文件與聯絡支援將很快提供。', notificationsSoon: '通知訊息將在後續版本接入。', ok: '知道了',
  },
  'ja-JP': {
    'nav.files': 'マイファイル', 'nav.recent': '最近使用した項目', 'nav.favorites': 'お気に入り', 'nav.settings': '設定', 'nav.trash': 'ごみ箱',
    space: 'マイスペース', disk: 'ディスク容量', personalSpace: '個人用スペース', search: 'ファイル、フォルダー、メンバーを検索',
    help: 'ヘルプ', notifications: '通知', close: '閉じる', default: '既定', sort: '並べ替え', sortModified: '更新日時', sortName: '名前', sortSize: 'ファイルサイズ',
    listView: 'リスト表示', gridView: 'グリッド表示', items: '項目', updatedToday: '最終更新: 今日', favoritesNote: 'お気に入りのファイル', trashNote: '復元または完全に削除できます',
    filesDescription: 'すべてのファイルはデバイスに自動同期されます', favoritesDescription: 'お気に入りのファイルがここに表示されます', trashDescription: '削除したファイルは 30 日後に完全に削除されます', settingsDescription: 'Aurora Drive の表示設定を調整します',
    newFolder: '新しいフォルダー', uploadFile: 'ファイルをアップロード', selected: '選択中', download: 'ダウンロード', move: '移動', share: '共有', favorite: 'お気に入りに追加', unfavorite: 'お気に入りから削除', delete: '削除', cancelSelection: '選択を解除',
    name: '名前', fileType: 'ファイルの種類', modifiedDate: '更新日時', deletedDate: '削除日時', fileSize: 'ファイルサイズ', selectAll: 'すべて選択',
    restore: '復元', permanentDelete: '完全に削除', emptyTrash: 'ごみ箱は空です', emptyFavorites: 'お気に入りのファイルはありません', loadingFiles: 'ファイル一覧を読み込んでいます', filesUnavailable: 'ファイルサービスに接続できません', emptyFiles: 'このフォルダーは空です',
    language: '言語', languageDescription: 'Aurora Drive の表示言語を選択', theme: 'テーマ', themeDescription: '明るさと外観を選択', aurora: 'Aurora', evernight: 'Evernight', auroraDescription: '明るい寒色', evernightDescription: 'ダークレッド',
    storageLoading: 'ディスク使用量を読み込んでいます', storageUnavailable: 'システムストレージサービスへの接続を待機中', refreshUsage: '使用量を更新', switchEvernight: 'Evernight モードに切り替え', switchAurora: 'Aurora モードに切り替え',
    folderTitle: '新しいフォルダー', folderDescription: 'ファイル用の新しい場所を作成します', folderLabel: 'フォルダー名', folderPlaceholder: '例：プロジェクト資料', cancel: 'キャンセル', createFolder: 'フォルダーを作成', uploadTitle: 'ファイルをアップロード', uploadDescription: 'ファイルは現在のフォルダーにアップロードされます', dropTitle: 'ファイルをここにドラッグしてアップロード', dropBrowse: 'またはクリックしてローカルファイルを選択', localFile: 'ローカルファイル', remove: '選択したファイルを削除', helpCenter: 'ヘルプセンター', helpDescription: 'ヘルプとサポート', notificationCenter: '通知センター', notificationDescription: 'システムメッセージと更新', inDevelopment: 'この機能は現在開発中です', helpSoon: 'ヘルプドキュメントとサポートはまもなく提供されます。', notificationsSoon: '通知は今後のバージョンで提供されます。', ok: '了解',
  },
  'en-US': {
    'nav.files': 'My files', 'nav.recent': 'Recent', 'nav.favorites': 'Favorites', 'nav.settings': 'Settings', 'nav.trash': 'Trash',
    space: 'My space', disk: 'Disk space', personalSpace: 'Personal space', search: 'Search files, folders, or members',
    help: 'Help', notifications: 'Notifications', close: 'Close', default: 'Default', sort: 'Sort by', sortModified: 'Last modified', sortName: 'Name', sortSize: 'File size',
    listView: 'List view', gridView: 'Grid view', items: 'items', updatedToday: 'Updated today', favoritesNote: 'Favorited files', trashNote: 'Restore or permanently delete',
    filesDescription: 'All files automatically sync to your devices', favoritesDescription: 'Your favorited files are collected here', trashDescription: 'Files are permanently removed 30 days after deletion', settingsDescription: 'Adjust Aurora Drive display preferences',
    newFolder: 'New folder', uploadFile: 'Upload file', selected: 'Selected', download: 'Download', move: 'Move', share: 'Share', favorite: 'Favorite', unfavorite: 'Unfavorite', delete: 'Delete', cancelSelection: 'Clear selection',
    name: 'Name', fileType: 'File type', modifiedDate: 'Modified', deletedDate: 'Deleted', fileSize: 'File size', selectAll: 'Select all items',
    restore: 'Restore', permanentDelete: 'Delete permanently', emptyTrash: 'Trash is empty', emptyFavorites: 'No favorited files', loadingFiles: 'Loading file list', filesUnavailable: 'Unable to connect to the file service', emptyFiles: 'This folder is empty',
    language: 'Language', languageDescription: 'Choose the display language for Aurora Drive', theme: 'Theme', themeDescription: 'Choose the interface appearance', aurora: 'Aurora', evernight: 'Evernight', auroraDescription: 'Light and cool', evernightDescription: 'Dark and rose',
    storageLoading: 'Reading disk usage', storageUnavailable: 'Waiting for the system storage service', refreshUsage: 'Refresh usage', switchEvernight: 'Switch to Evernight mode', switchAurora: 'Switch to Aurora mode',
    folderTitle: 'New folder', folderDescription: 'Create a new place for your files', folderLabel: 'Folder name', folderPlaceholder: 'For example: Project assets', cancel: 'Cancel', createFolder: 'Create folder', uploadTitle: 'Upload file', uploadDescription: 'The file will be uploaded to the current folder', dropTitle: 'Drag a file here to upload', dropBrowse: 'or click here to browse local files', localFile: 'Local file', remove: 'Remove selected file', helpCenter: 'Help center', helpDescription: 'Help and support', notificationCenter: 'Notification center', notificationDescription: 'System messages and activity', inDevelopment: 'This feature is in development', helpSoon: 'Help documentation and support will be available soon.', notificationsSoon: 'Notifications will be available in a future release.', ok: 'Got it',
  },
  'fr-FR': {
    'nav.files': 'Mes fichiers', 'nav.recent': 'Récents', 'nav.favorites': 'Favoris', 'nav.settings': 'Paramètres', 'nav.trash': 'Corbeille',
    space: 'Mon espace', disk: 'Espace disque', personalSpace: 'Espace personnel', search: 'Rechercher des fichiers, dossiers ou membres',
    help: 'Aide', notifications: 'Notifications', close: 'Fermer', default: 'Par défaut', sort: 'Trier par', sortModified: 'Dernière modification', sortName: 'Nom', sortSize: 'Taille du fichier',
    listView: 'Vue en liste', gridView: 'Vue en grille', items: 'éléments', updatedToday: "Mis à jour aujourd'hui", favoritesNote: 'Fichiers favoris', trashNote: 'Restaurer ou supprimer définitivement',
    filesDescription: 'Tous les fichiers sont automatiquement synchronisés sur vos appareils', favoritesDescription: 'Vos fichiers favoris sont rassemblés ici', trashDescription: 'Les fichiers sont supprimés définitivement 30 jours après leur suppression', settingsDescription: "Ajustez les préférences d'affichage d'Aurora Drive",
    newFolder: 'Nouveau dossier', uploadFile: 'Importer un fichier', selected: 'Sélectionnés', download: 'Télécharger', move: 'Déplacer', share: 'Partager', favorite: 'Ajouter aux favoris', unfavorite: 'Retirer des favoris', delete: 'Supprimer', cancelSelection: 'Annuler la sélection',
    name: 'Nom', fileType: 'Type de fichier', modifiedDate: 'Modifié', deletedDate: 'Supprimé', fileSize: 'Taille du fichier', selectAll: 'Sélectionner tous les éléments',
    restore: 'Restaurer', permanentDelete: 'Supprimer définitivement', emptyTrash: 'La corbeille est vide', emptyFavorites: 'Aucun fichier favori', loadingFiles: 'Chargement de la liste des fichiers', filesUnavailable: 'Impossible de joindre le service de fichiers', emptyFiles: 'Ce dossier est vide',
    language: 'Langue', languageDescription: "Choisissez la langue d'affichage d'Aurora Drive", theme: 'Thème', themeDescription: "Choisissez l'apparence de l'interface", aurora: 'Aurora', evernight: 'Evernight', auroraDescription: 'Clair et froid', evernightDescription: 'Sombre et rose',
    storageLoading: "Lecture de l'utilisation du disque", storageUnavailable: 'En attente du service de stockage système', refreshUsage: "Actualiser l'utilisation", switchEvernight: 'Passer en mode Evernight', switchAurora: 'Passer en mode Aurora',
    folderTitle: 'Nouveau dossier', folderDescription: 'Créez un nouvel emplacement pour vos fichiers', folderLabel: 'Nom du dossier', folderPlaceholder: 'Par exemple : Ressources du projet', cancel: 'Annuler', createFolder: 'Créer le dossier', uploadTitle: 'Importer un fichier', uploadDescription: 'Le fichier sera importé dans le dossier actuel', dropTitle: 'Déposez un fichier ici pour l’importer', dropBrowse: 'ou cliquez ici pour parcourir les fichiers locaux', localFile: 'Fichier local', remove: 'Retirer le fichier sélectionné', helpCenter: "Centre d'aide", helpDescription: 'Aide et assistance', notificationCenter: 'Centre de notifications', notificationDescription: 'Messages et activité système', inDevelopment: 'Cette fonctionnalité est en cours de développement', helpSoon: "La documentation et l'assistance seront bientôt disponibles.", notificationsSoon: 'Les notifications arriveront dans une prochaine version.', ok: 'Compris',
  },
};

interface DriveFile {
  id: number;
  name: string;
  kind: FileKind;
  type: string;
  modified: string;
  size: string;
  deletedAt?: string;
  favorited?: boolean;
}

interface StorageUsage {
  usedBytes: number;
  totalBytes: number;
}

interface FileApiEntry {
  name: string;
  type: string;
  size: number;
  last_modified: string;
  directory: boolean;
}

interface FileBrowseResponse {
  success: boolean;
  entries: FileApiEntry[];
}

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css'
})
export class App {
  protected readonly title = signal('aurora-drive');
  protected readonly activeSection = signal<Section>('files');
  protected readonly searchQuery = signal('');
  protected readonly sortBy = signal<SortBy>('server');
  protected readonly viewMode = signal<'list' | 'grid'>('list');
  protected readonly selectedIds = signal<Set<number>>(new Set());
  protected readonly showFolderForm = signal(false);
  protected readonly showUploadDialog = signal(false);
  protected readonly newFolderName = signal('');
  protected readonly selectedUploadFile = signal<File | null>(null);
  protected readonly sidebarCollapsed = signal(false);
  protected readonly theme = signal<Theme>('aurora');
  protected readonly locale = signal<Locale>(this.getInitialLocale());
  protected readonly supportedLocales = supportedLocales;
  protected readonly placeholderDialog = signal<PlaceholderDialog | null>(null);
  protected readonly storageUsage = signal<StorageUsage | null>(null);
  protected readonly storageStatus = signal<'loading' | 'ready' | 'unavailable'>('loading');
  protected readonly filesStatus = signal<'loading' | 'ready' | 'unavailable'>('loading');

  protected readonly files = signal<DriveFile[]>([]);
  protected readonly trashFiles = signal<DriveFile[]>([]);

  protected readonly isTrashSection = computed(() => this.activeSection() === 'trash');
  protected readonly isFavoritesSection = computed(() => this.activeSection() === 'favorites');
  protected readonly isSettingsSection = computed(() => this.activeSection() === 'settings');
  protected readonly sectionTitle = computed(() => this.t(`nav.${this.activeSection()}`));
  protected readonly sectionDescription = computed(() => {
    if (this.isTrashSection()) return this.t('trashDescription');
    if (this.isFavoritesSection()) return this.t('favoritesDescription');
    if (this.isSettingsSection()) return this.t('settingsDescription');
    return this.t('filesDescription');
  });
  protected readonly storagePercentage = computed(() => {
    const usage = this.storageUsage();
    return usage ? Math.min(100, Math.round((usage.usedBytes / usage.totalBytes) * 100)) : 0;
  });

  constructor() {
    this.updateDocumentLocale();
    void this.refreshStorageUsage();
    void this.refreshFiles();
  }

  protected readonly visibleFiles = computed(() => {
    const query = this.searchQuery().trim().toLowerCase();
    const sectionFiles = this.isTrashSection()
      ? this.trashFiles()
      : this.isFavoritesSection()
        ? this.files().filter((file) => file.favorited)
        : this.files();
    const files = sectionFiles.filter((file) =>
      !query || file.name.toLowerCase().includes(query) || file.type.toLowerCase().includes(query),
    );

    if (this.sortBy() === 'server') return files;

    return [...files].sort((left, right) => {
      if (this.sortBy() === 'name') return left.name.localeCompare(right.name, 'zh-CN');
      if (this.sortBy() === 'size') return left.size.localeCompare(right.size);
      return right.modified.localeCompare(left.modified, 'zh-CN');
    });
  });

  protected readonly selectedCount = computed(() => this.selectedIds().size);
  protected readonly selectedFilesAreFavorited = computed(() => {
    const selected = this.selectedIds();
    return selected.size > 0 && [...selected].every((fileId) => this.files().some((file) => file.id === fileId && file.favorited));
  });
  protected readonly allVisibleSelected = computed(() => {
    const visible = this.visibleFiles();
    return visible.length > 0 && visible.every((file) => this.selectedIds().has(file.id));
  });

  protected setActiveSection(section: Section): void {
    this.activeSection.set(section);
    this.clearSelection();
  }

  protected toggleSidebar(): void {
    this.sidebarCollapsed.update((collapsed) => !collapsed);
  }

  protected toggleTheme(): void {
    this.theme.update((theme) => theme === 'aurora' ? 'evernight' : 'aurora');
  }

  protected setTheme(theme: Theme): void {
    this.theme.set(theme);
  }

  protected updateLocale(event: Event): void {
    const locale = (event.target as HTMLSelectElement).value as Locale;
    if (!supportedLocales.some((supportedLocale) => supportedLocale.code === locale)) return;
    this.locale.set(locale);
    this.saveLocale(locale);
    this.updateDocumentLocale();
  }

  protected t(key: string): string {
    return translations[this.locale()][key] ?? translations['en-US'][key] ?? key;
  }

  private detectLocale(): Locale {
    const browserLocales = typeof navigator === 'undefined'
      ? []
      : navigator.languages?.length
        ? navigator.languages
        : [navigator.language];

    for (const browserLocale of browserLocales) {
      try {
        const locale = new Intl.Locale(browserLocale);
        const candidate = locale.region ? `${locale.language}-${locale.region}`.toLowerCase() : '';
        const matchedLocale = supportedLocales.find((supportedLocale) => supportedLocale.code.toLowerCase() === candidate);
        if (matchedLocale) return matchedLocale.code;
      } catch {
        continue;
      }
    }
    return 'en-US';
  }

  private getInitialLocale(): Locale {
    const savedLocale = typeof localStorage === 'undefined' ? null : localStorage.getItem(localeStorageKey);
    return supportedLocales.some((supportedLocale) => supportedLocale.code === savedLocale)
      ? savedLocale as Locale
      : this.detectLocale();
  }

  private saveLocale(locale: Locale): void {
    if (typeof localStorage !== 'undefined') localStorage.setItem(localeStorageKey, locale);
  }

  private updateDocumentLocale(): void {
    if (typeof document !== 'undefined') document.documentElement.lang = this.locale();
  }

  protected openPlaceholderDialog(dialog: PlaceholderDialog): void {
    this.placeholderDialog.set(dialog);
  }

  protected closePlaceholderDialog(): void {
    this.placeholderDialog.set(null);
  }

  protected async refreshStorageUsage(): Promise<void> {
    this.storageStatus.set('loading');
    try {
      const response = await fetch('/api/system/storage');
      if (!response.ok) throw new Error('Storage endpoint unavailable');

      const data = await response.json() as StorageUsage;
      if (!Number.isFinite(data.usedBytes) || !Number.isFinite(data.totalBytes) || data.totalBytes <= 0) {
        throw new Error('Invalid storage response');
      }
      this.storageUsage.set(data);
      this.storageStatus.set('ready');
    } catch {
      this.storageUsage.set(null);
      this.storageStatus.set('unavailable');
    }
  }

  protected async refreshFiles(): Promise<void> {
    this.filesStatus.set('loading');
    try {
      const response = await fetch('/api/files');
      if (!response.ok) throw new Error('File endpoint unavailable');

      const data = await response.json() as Partial<FileBrowseResponse>;
      if (data.success !== true || !Array.isArray(data.entries)) throw new Error('Invalid file response');

      this.files.set(data.entries.map((entry, index) => this.toDriveFile(entry, index)));
      this.filesStatus.set('ready');
    } catch {
      this.files.set([]);
      this.filesStatus.set('unavailable');
    }
  }

  protected openFolderDialog(): void {
    this.showFolderForm.set(true);
  }

  protected closeFolderDialog(): void {
    this.newFolderName.set('');
    this.showFolderForm.set(false);
  }

  protected openUploadDialog(): void {
    this.showUploadDialog.set(true);
  }

  protected closeUploadDialog(): void {
    this.selectedUploadFile.set(null);
    this.showUploadDialog.set(false);
  }

  protected updateSearch(event: Event): void {
    this.searchQuery.set((event.target as HTMLInputElement).value);
  }

  protected updateSort(event: Event): void {
    this.sortBy.set((event.target as HTMLSelectElement).value as SortBy);
  }

  protected toggleFile(fileId: number): void {
    const nextSelection = new Set(this.selectedIds());
    nextSelection.has(fileId) ? nextSelection.delete(fileId) : nextSelection.add(fileId);
    this.selectedIds.set(nextSelection);
  }

  protected toggleAll(): void {
    const nextSelection = new Set(this.selectedIds());
    if (this.allVisibleSelected()) {
      this.visibleFiles().forEach((file) => nextSelection.delete(file.id));
    } else {
      this.visibleFiles().forEach((file) => nextSelection.add(file.id));
    }
    this.selectedIds.set(nextSelection);
  }

  protected clearSelection(): void {
    this.selectedIds.set(new Set());
  }

  protected toggleFavorite(fileId: number): void {
    this.files.update((files) => files.map((file) => file.id === fileId ? { ...file, favorited: !file.favorited } : file));
  }

  protected favoriteSelectedFiles(): void {
    const selected = this.selectedIds();
    this.files.update((files) => files.map((file) => selected.has(file.id) ? { ...file, favorited: true } : file));
  }

  protected unfavoriteSelectedFiles(): void {
    const selected = this.selectedIds();
    this.files.update((files) => files.map((file) => selected.has(file.id) ? { ...file, favorited: false } : file));
    this.clearSelection();
  }

  protected moveSelectedToTrash(): void {
    const selected = this.selectedIds();
    const removedFiles = this.files().filter((file) => selected.has(file.id));
    if (!removedFiles.length) return;

    this.files.update((files) => files.filter((file) => !selected.has(file.id)));
    this.trashFiles.update((files) => [
      ...removedFiles.map((file) => ({ ...file, deletedAt: '刚刚' })),
      ...files,
    ]);
    this.clearSelection();
  }

  protected restoreFile(fileId: number): void {
    const file = this.trashFiles().find((item) => item.id === fileId);
    if (!file) return;

    const { deletedAt, ...restoredFile } = file;
    this.files.update((files) => [{ ...restoredFile, modified: '刚刚' }, ...files]);
    this.trashFiles.update((files) => files.filter((item) => item.id !== fileId));
    this.selectedIds.update((ids) => {
      const nextIds = new Set(ids);
      nextIds.delete(fileId);
      return nextIds;
    });
  }

  protected permanentlyDeleteFile(fileId: number): void {
    this.trashFiles.update((files) => files.filter((file) => file.id !== fileId));
    this.selectedIds.update((ids) => {
      const nextIds = new Set(ids);
      nextIds.delete(fileId);
      return nextIds;
    });
  }

  protected updateNewFolderName(event: Event): void {
    this.newFolderName.set((event.target as HTMLInputElement).value);
  }

  protected selectUploadFile(event: Event): void {
    this.selectedUploadFile.set((event.target as HTMLInputElement).files?.item(0) ?? null);
  }

  protected selectDroppedFile(event: DragEvent): void {
    event.preventDefault();
    this.selectedUploadFile.set(event.dataTransfer?.files.item(0) ?? null);
  }

  protected uploadSelectedFile(): void {
    const file = this.selectedUploadFile();
    if (!file) return;

    this.files.update((files) => [
      {
        id: Date.now(),
        name: file.name,
        kind: 'document',
        type: this.getFileType(file.name),
        modified: '刚刚',
        size: this.formatFileSize(file.size),
      },
      ...files,
    ]);
    this.closeUploadDialog();
  }

  protected createFolder(): void {
    const name = this.newFolderName().trim();
    if (!name) return;

    this.files.update((files) => [
      { id: Date.now(), name, kind: 'folder', type: '文件夹', modified: '刚刚', size: '--' },
      ...files,
    ]);
    this.closeFolderDialog();
  }

  private getFileType(fileName: string): string {
    const extension = fileName.split('.').pop()?.toLowerCase();
    if (['png', 'jpg', 'jpeg', 'gif', 'webp'].includes(extension ?? '')) return '图片';
    if (['mp4', 'mov', 'webm'].includes(extension ?? '')) return '视频';
    if (['zip', 'rar', '7z'].includes(extension ?? '')) return '压缩包';
    if (['xlsx', 'xls', 'csv'].includes(extension ?? '')) return '表格';
    return '文档';
  }

  private toDriveFile(entry: FileApiEntry, index: number): DriveFile {
    return {
      id: index + 1,
      name: entry.name,
      kind: this.getFileKind(entry.name, entry.directory),
      type: entry.type,
      modified: entry.last_modified,
      size: entry.directory ? '--' : this.formatFileSize(entry.size),
    };
  }

  private getFileKind(fileName: string, isDirectory: boolean): FileKind {
    if (isDirectory) return 'folder';

    const extension = fileName.split('.').pop()?.toLowerCase();
    if (['png', 'jpg', 'jpeg', 'gif', 'webp', 'bmp', 'svg', 'heic'].includes(extension ?? '')) return 'image';
    if (['mp4', 'mov', 'webm', 'avi', 'mkv'].includes(extension ?? '')) return 'video';
    if (['zip', 'rar', '7z', 'tar', 'gz', 'bz2', 'xz', 'zst'].includes(extension ?? '')) return 'archive';
    return 'document';
  }

  private formatFileSize(size: number): string {
    if (size === 0) return '0 B';
    if (size < 1024 * 1024) return `${Math.max(1, Math.round(size / 1024))} KB`;
    return `${(size / (1024 * 1024)).toFixed(1)} MB`;
  }

  protected formatStorageSize(size: number): string {
    if (size < 1024 * 1024 * 1024) return `${Math.round(size / (1024 * 1024))} MB`;
    return `${(size / (1024 * 1024 * 1024)).toFixed(1)} GB`;
  }
}
