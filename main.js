(function () {
  // helpers
  const isMobile = /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent) || window.innerWidth < 840;
  if (isMobile) document.body.classList.add('mobile');

  // elements
  const menuToggle = document.getElementById('menuToggle'); // 设置按钮
  const toolsMenu = document.getElementById('toolsMenu'); // 设置浮窗
  const settingPlayer = document.getElementById('setting_player');
  const settingReduce = document.getElementById('setting_reduce');
  const saveSettings = document.getElementById('saveSettings');

  const aboutBtn = document.getElementById('aboutBtn'); // 关于按钮
  const aboutModalOverlay = document.getElementById('aboutModalOverlay'); // 关于浮窗蒙层
  const closeAboutModal = document.getElementById('closeAboutModal'); // 关闭关于浮窗按钮

  // music elements
  const musicPlayer = document.getElementById('musicPlayer');
  const disc = document.getElementById('disc');
  const playerControls = document.getElementById('playerControls');
  const playPauseBtn = document.getElementById('playPauseBtn');
  const nextTrackBtn = document.getElementById('nextTrackBtn'); // 新增：下一首按钮
  const volumeSlider = document.getElementById('volumeSlider'); // 新增：音量滑块
  const bgm = document.getElementById('bgm');
  const progressBar = document.getElementById('progressBar');
  const timeLabel = document.getElementById('timeLabel');

  // New autoplay modal elements
  const autoplayModalOverlay = document.getElementById('autoplayModalOverlay');
  const closeAutoplayModalBtn = document.getElementById('closeAutoplayModalBtn');

  const SETTINGS_KEY = 'summer_settings_v1';
  const MUSIC_STATE_KEY = 'summer_player_state_v2'; // 更新版本号以避免冲突
  const AUTOPLAY_KEY = 'summer_autoplay_enabled';

  const defaultSettings = {
    player: true,
    reducedMotion: false
  };

  const musicTracks = [
    'snd/bgm_1.mp3',
    'snd/bgm_2.mp3',
    'snd/bgm_3.mp3',
    'snd/bgm_4.mp3',
    'snd/bgm_5.mp3',
    'snd/bgm_6.mp3',
    'snd/bgm_7.mp3',
    'snd/bgm_8.mp3',
    'snd/bgm_9.mp3',
  ];

  let currentTrackIndex = -1; // 初始化为-1，表示未选择任何歌曲

  function loadSettings() {
    try {
      const raw = localStorage.getItem(SETTINGS_KEY);
      const parsed = raw ? JSON.parse(raw) : defaultSettings;
      const settings = Object.assign({}, defaultSettings, parsed);
      settingPlayer.checked = !!settings.player;
      settingReduce.checked = !!settings.reducedMotion;
      applySettings(settings);
      return settings;
    } catch (e) {
      console.warn('读取设置失败', e);
      settingPlayer.checked = defaultSettings.player;
      settingReduce.checked = defaultSettings.reducedMotion;
      applySettings(defaultSettings);
      return defaultSettings;
    }
  }

  function saveSettingsToStorage(settings) {
    try {
      localStorage.setItem(SETTINGS_KEY, JSON.stringify(settings));
    } catch (e) {
      console.warn('保存设置失败', e);
    }
  }

  function applySettings(settings) {
    if (settings.player) musicPlayer.style.display = '';
    else {
      musicPlayer.style.display = 'none';
      playerControls.style.display = 'none';
    }

    const animatedElements = document.querySelectorAll('.card, .cta, .action-button, .disc');
    if (settings.reducedMotion) {
      document.documentElement.classList.add('reduced-motion');
      animatedElements.forEach((el) => {
        el.style.transition = 'none';
        el.style.animation = 'none';
      });
      disc.classList.remove('playing');
    } else {
      document.documentElement.classList.remove('reduced-motion');
      animatedElements.forEach((el) => {
        el.style.transition = '';
        el.style.animation = '';
      });
      if (!bgm.paused) {
        disc.classList.add('playing');
      }
    }
  }

  // 音乐播放器逻辑
  function getRandomTrackIndex(excludeIndex) {
    let newIndex;
    do {
      newIndex = Math.floor(Math.random() * musicTracks.length);
    } while (newIndex === excludeIndex && musicTracks.length > 1);
    return newIndex;
  }

  function playNextRandomTrack() {
    const previousIndex = currentTrackIndex;
    currentTrackIndex = getRandomTrackIndex(previousIndex);
    bgm.src = musicTracks[currentTrackIndex];
    bgm.play();
    saveMusicState();
  }

  function loadMusicState() {
    try {
      const raw = localStorage.getItem(MUSIC_STATE_KEY);
      const state = raw ? JSON.parse(raw) : {};

      if (state.currentTrackIndex !== undefined && state.currentTrackIndex < musicTracks.length) {
        currentTrackIndex = state.currentTrackIndex;
      } else {
        currentTrackIndex = getRandomTrackIndex(-1);
      }

      bgm.src = musicTracks[currentTrackIndex];
      bgm.volume = state.volume !== undefined ? state.volume : 0.5;
      volumeSlider.value = bgm.volume;

      if (state.currentTime) {
        bgm.currentTime = state.currentTime;
      }

      const currentSettings = loadSettings();
      // Only try to play if user settings allow it and a past state exists
      if (currentSettings.player && state.playing) {
          bgm.play().catch(() => { /* autoplay blocked or failed, do nothing */ });
      }

    } catch (e) {
      console.warn('读取音乐状态失败', e);
      currentTrackIndex = getRandomTrackIndex(-1);
      bgm.src = musicTracks[currentTrackIndex];
      volumeSlider.value = 0.5;
    }
  }

  function saveMusicState() {
    try {
      localStorage.setItem(MUSIC_STATE_KEY, JSON.stringify({
        playing: !bgm.paused,
        currentTime: bgm.currentTime,
        currentTrackIndex: currentTrackIndex,
        volume: bgm.volume
      }));
    } catch (e) {
      console.warn('保存音乐状态失败', e);
    }
  }

  // init - check for autoplay state
  const autoplayEnabled = localStorage.getItem(AUTOPLAY_KEY);

  if (autoplayEnabled) {
    autoplayModalOverlay.style.display = 'none';
    loadMusicState();
  } else {
    // Show modal and wait for user interaction
    autoplayModalOverlay.style.display = 'flex';
    // Load music state but don't play yet
    loadMusicState();
  }

  closeAutoplayModalBtn.addEventListener('click', () => {
      // Hide modal
      autoplayModalOverlay.style.display = 'none';
      // Save state to avoid showing it again
      localStorage.setItem(AUTOPLAY_KEY, 'true');
      // Attempt to play music after user interaction
      bgm.play().catch(e => console.warn('自动播放失败', e));
  });


  saveSettings.addEventListener('click', () => {
    const newS = {
      player: !!settingPlayer.checked,
      reducedMotion: !!settingReduce.checked
    };
    applySettings(newS);
    saveSettingsToStorage(newS);
    saveSettings.textContent = '已保存';
    setTimeout(() => (saveSettings.textContent = '保存'), 900);
  });

  // menu toggle
  menuToggle.addEventListener('click', () => {
    const expanded = menuToggle.getAttribute('aria-expanded') === 'true';
    menuToggle.setAttribute('aria-expanded', String(!expanded));
    if (expanded) {
      toolsMenu.style.display = 'none';
      toolsMenu.setAttribute('aria-hidden', 'true');
    } else {
      toolsMenu.style.display = 'block';
      toolsMenu.setAttribute('aria-hidden', 'false');
    }
  });

  // close menus and modals when clicking outside
  document.addEventListener('click', (e) => {
    // Hide settings menu if click is outside it and its toggle button
    if (!toolsMenu.contains(e.target) && !menuToggle.contains(e.target)) {
      toolsMenu.style.display = 'none';
      toolsMenu.setAttribute('aria-hidden', 'true');
      menuToggle.setAttribute('aria-expanded', 'false');
    }
    // Hide about modal if click is outside it and its toggle button
    if (aboutModalOverlay.classList.contains('active') && !aboutModalOverlay.contains(e.target) && !aboutBtn.contains(e.target)) {
      aboutModalOverlay.classList.remove('active');
      aboutBtn.setAttribute('aria-expanded', 'false');
    }
    // Hide player controls if click is outside them and the music player button
    if (playerControls.style.display === 'flex' && !playerControls.contains(e.target) && !musicPlayer.contains(e.target)) {
      playerControls.style.display = 'none';
      playerControls.setAttribute('aria-hidden', 'true');
    }
  });

  // music player interaction
  musicPlayer.addEventListener('click', (e) => {
    const controlsVisible = playerControls.style.display === 'flex';
    playerControls.style.display = controlsVisible ? 'none' : 'flex';
    playerControls.setAttribute('aria-hidden', String(!controlsVisible));
  });

  // play/pause logic
  function setPlayingState(isPlaying) {
    if (isPlaying) {
      const currentSettings = loadSettings();
      if (!currentSettings.reducedMotion) {
        disc.classList.add('playing');
      }
      playPauseBtn.textContent = '⏸️';
      playPauseBtn.setAttribute('aria-pressed', 'true');
    } else {
      disc.classList.remove('playing');
      playPauseBtn.textContent = '▶️';
      playPauseBtn.setAttribute('aria-pressed', 'false');
    }
    saveMusicState();
  }

  playPauseBtn.addEventListener('click', () => {
    if (bgm.paused) {
      bgm.play();
    } else {
      bgm.pause();
    }
  });

  // 新增：下一首功能
  nextTrackBtn.addEventListener('click', playNextRandomTrack);

  // 新增：音量控制
  volumeSlider.addEventListener('input', () => {
    bgm.volume = volumeSlider.value;
    saveMusicState();
  });

  bgm.addEventListener('play', () => {
    setPlayingState(true);
  });
  bgm.addEventListener('pause', () => {
    setPlayingState(false);
  });
  bgm.addEventListener('ended', () => {
    playNextRandomTrack();
  });
  bgm.addEventListener('timeupdate', () => {
    if (!bgm.duration || isNaN(bgm.duration)) return;
    const pct = Math.max(0, Math.min(100, (bgm.currentTime / bgm.duration) * 100));
    progressBar.style.width = pct + '%';
    const mm = Math.floor(bgm.currentTime / 60),
      ss = Math.floor(bgm.currentTime % 60);
    timeLabel.textContent = mm + ':' + (ss < 10 ? '0' + ss : ss);
    saveMusicState();
  });

  // click progress to seek
  playerControls.querySelector('.progress').addEventListener('click', (e) => {
    const rect = e.currentTarget.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const pct = x / rect.width;
    if (bgm.duration) bgm.currentTime = Math.max(0, Math.min(bgm.duration, pct * bgm.duration));
  });

  // toggle disc play by double click
  disc.addEventListener('dblclick', () => {
    if (bgm.paused) bgm.play();
    else bgm.pause();
  });

  // keyboard shortcuts: space toggles player when focused
  document.addEventListener('keydown', (e) => {
    if (e.code === 'Space' && document.activeElement.tagName !== 'INPUT' && document.activeElement.tagName !== 'TEXTAREA') {
      e.preventDefault();
      if (bgm.paused) bgm.play();
      else bgm.pause();
    }
  });

  // Accessibility: focus trapping for menu when open (simple)
  toolsMenu.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
      toolsMenu.style.display = 'none';
      menuToggle.setAttribute('aria-expanded', 'false');
    }
  });

  // About Modal Logic
  aboutBtn.addEventListener('click', () => {
    const expanded = aboutBtn.getAttribute('aria-expanded') === 'true';
    aboutBtn.setAttribute('aria-expanded', String(!expanded));
    if (expanded) {
      aboutModalOverlay.classList.remove('active');
      aboutModalOverlay.setAttribute('aria-hidden', 'true');
    } else {
      aboutModalOverlay.classList.add('active');
      aboutModalOverlay.setAttribute('aria-hidden', 'false');
      setTimeout(() => closeAboutModal.focus(), 100);
    }
  });

  closeAboutModal.addEventListener('click', () => {
    aboutModalOverlay.classList.remove('active');
    aboutModalOverlay.setAttribute('aria-hidden', 'true');
    aboutBtn.setAttribute('aria-expanded', 'false');
  });

  // Close modal on escape key
  aboutModalOverlay.addEventListener('keydown', (e) => {
    if (e.key === 'Escape' && aboutModalOverlay.classList.contains('active')) {
      closeAboutModal.click();
    }
  });

  // ensure player visibility obeys settings on resize too (mobile detection)
  window.addEventListener('resize', () => {
    const mobileNow = /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent) || window.innerWidth < 840;
    if (mobileNow) document.body.classList.add('mobile');
    else document.body.classList.remove('mobile');
    applySettings(loadSettings());
  });

  if (!bgm.paused && !bgm.ended) setPlayingState(true);
})();