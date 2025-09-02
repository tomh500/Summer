(function () {
  // helpers
  const isMobile = /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent) || window.innerWidth < 840;
  if (isMobile) document.body.classList.add('mobile');

  // elements
  const menuToggle = document.getElementById('menuToggle');
  const toolsMenu = document.getElementById('toolsMenu');
  const settingPlayer = document.getElementById('setting_player');
  const settingReduce = document.getElementById('setting_reduce');
  const saveSettings = document.getElementById('saveSettings');

  // music elements
  const musicPlayer = document.getElementById('musicPlayer');
  const disc = document.getElementById('disc');
  const playerControls = document.getElementById('playerControls');
  const playPauseBtn = document.getElementById('playPauseBtn');
  const bgm = document.getElementById('bgm');
  const progressBar = document.getElementById('progressBar');
  const timeLabel = document.getElementById('timeLabel');

  const SETTINGS_KEY = 'summer_settings_v1';
  const defaultSettings = {
    player: true,
    reducedMotion: false
  };

  function loadSettings() {
    try {
      const raw = localStorage.getItem(SETTINGS_KEY);
      const parsed = raw ? JSON.parse(raw) : defaultSettings;
      // merge defaults
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
    // show/hide player
    if (settings.player) musicPlayer.style.display = '';
    else {
      musicPlayer.style.display = 'none';
      playerControls.style.display = 'none';
    }

    // reduced motion
    if (settings.reducedMotion) {
      document.documentElement.style.setProperty('--reduce-motion', '1');
      document.querySelectorAll('.card, .cta, .hamburger, .disc').forEach((el) => (el.style.transition = 'none'));
      document.documentElement.classList.add('reduced-motion');
    } else {
      document.documentElement.style.removeProperty('--reduce-motion');
      document.querySelectorAll('.card, .cta, .hamburger, .disc').forEach((el) => (el.style.transition = ''));
      document.documentElement.classList.remove('reduced-motion');
    }
  }

  // init
  const currentSettings = loadSettings();

  saveSettings.addEventListener('click', () => {
    const newS = {
      player: !!settingPlayer.checked,
      reducedMotion: !!settingReduce.checked
    };
    applySettings(newS);
    saveSettingsToStorage(newS);
    // small feedback
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

  // close menu when clicking outside
  document.addEventListener('click', (e) => {
    if (!toolsMenu.contains(e.target) && !menuToggle.contains(e.target)) {
      toolsMenu.style.display = 'none';
      toolsMenu.setAttribute('aria-hidden', 'true');
      menuToggle.setAttribute('aria-expanded', 'false');
    }
  });

  // music player interaction
  let controlsVisible = false;
  musicPlayer.addEventListener('click', (e) => {
    controlsVisible = !controlsVisible;
    playerControls.style.display = controlsVisible ? 'block' : 'none';
    playerControls.setAttribute('aria-hidden', String(!controlsVisible));
  });

  // play/pause logic
  function setPlayingState(isPlaying) {
    if (isPlaying) {
      disc.classList.add('playing');
      playPauseBtn.textContent = '⏸';
      playPauseBtn.setAttribute('aria-pressed', 'true');
    } else {
      disc.classList.remove('playing');
      playPauseBtn.textContent = '▶️';
      playPauseBtn.setAttribute('aria-pressed', 'false');
    }
  }

  // restore playback state if any
  try {
    const st = localStorage.getItem('summer_player_state');
    if (st && JSON.parse(st).playing) {
      bgm.play().catch(() => {
        /* autoplay blocked */
      });
    }
  } catch (e) {}

  playPauseBtn.addEventListener('click', () => {
    if (bgm.paused) {
      bgm.play();
    } else {
      bgm.pause();
    }
  });

  bgm.addEventListener('play', () => {
    setPlayingState(true);
    try {
      localStorage.setItem('summer_player_state', JSON.stringify({
        playing: true
      }));
    } catch (e) {}
  });
  bgm.addEventListener('pause', () => {
    setPlayingState(false);
    try {
      localStorage.setItem('summer_player_state', JSON.stringify({
        playing: false
      }));
    } catch (e) {}
  });
  bgm.addEventListener('timeupdate', () => {
    if (!bgm.duration || isNaN(bgm.duration)) return;
    const pct = Math.max(0, Math.min(100, (bgm.currentTime / bgm.duration) * 100));
    progressBar.style.width = pct + '%';
    const mm = Math.floor(bgm.currentTime / 60),
      ss = Math.floor(bgm.currentTime % 60);
    timeLabel.textContent = mm + ':' + (ss < 10 ? '0' + ss : ss);
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

  // ensure player visibility obeys settings on resize too (mobile detection)
  window.addEventListener('resize', () => {
    const mobileNow = /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent) || window.innerWidth < 840;
    if (mobileNow) document.body.classList.add('mobile');
    else document.body.classList.remove('mobile');
  });

  // On first load, if player disabled by settings hide it
  if (!currentSettings.player) {
    musicPlayer.style.display = 'none';
  }

  // Set initial state for disc rotation (if audio already playing)
  if (!bgm.paused && !bgm.ended) setPlayingState(true);
})();