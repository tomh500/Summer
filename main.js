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
  const bgm = document.getElementById('bgm');
  const progressBar = document.getElementById('progressBar');
  const timeLabel = document.getElementById('timeLabel');

  const SETTINGS_KEY = 'summer_settings_v1';
  const MUSIC_STATE_KEY = 'summer_player_state'; // 用于保存播放状态和当前歌曲索引

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

  let currentTrackIndex = 0; // 当前播放歌曲的索引

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
    const animatedElements = document.querySelectorAll('.card, .cta, .action-button, .disc');
    if (settings.reducedMotion) {
      document.documentElement.classList.add('reduced-motion'); // 添加class，可以通过CSS控制
      // 禁用所有相关元素的CSS transition和animation
      animatedElements.forEach((el) => {
        el.style.transition = 'none';
        el.style.animation = 'none';
      });
      disc.classList.remove('playing'); // 停止碟片旋转动画
    } else {
      document.documentElement.classList.remove('reduced-motion');
      animatedElements.forEach((el) => {
        el.style.transition = ''; // 恢复默认transition
        el.style.animation = ''; // 恢复默认animation
      });
      if (!bgm.paused) { // 如果音乐正在播放，恢复碟片旋转
        disc.classList.add('playing');
      }
    }
  }

  // 音乐播放器逻辑
  function getRandomTrackIndex(excludeIndex) {
    let newIndex;
    do {
      newIndex = Math.floor(Math.random() * musicTracks.length);
    } while (newIndex === excludeIndex && musicTracks.length > 1); // 确保不是同一首，除非只有一首
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
      if (raw) {
        const state = JSON.parse(raw);
        if (state.currentTrackIndex !== undefined && state.currentTrackIndex < musicTracks.length) {
          currentTrackIndex = state.currentTrackIndex;
        }
        bgm.src = musicTracks[currentTrackIndex]; // 加载上次播放的歌曲
        if (state.currentTime) {
          bgm.currentTime = state.currentTime;
        }
        if (state.playing) {
          // 如果上次是播放状态，尝试自动播放
          bgm.play().catch(() => { /* autoplay blocked */ });
        }
      } else {
        // 第一次加载或没有保存状态，随机播放一首
        currentTrackIndex = getRandomTrackIndex(-1); // -1确保第一次随机
        bgm.src = musicTracks[currentTrackIndex];
      }
    } catch (e) {
      console.warn('读取音乐状态失败', e);
      // 失败时也随机一首
      currentTrackIndex = getRandomTrackIndex(-1);
      bgm.src = musicTracks[currentTrackIndex];
    }
  }

  function saveMusicState() {
    try {
      localStorage.setItem(MUSIC_STATE_KEY, JSON.stringify({
        playing: !bgm.paused,
        currentTime: bgm.currentTime,
        currentTrackIndex: currentTrackIndex
      }));
    } catch (e) {
      console.warn('保存音乐状态失败', e);
    }
  }

  // init
  const currentSettings = loadSettings();
  loadMusicState(); // 加载音乐状态

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
    // 检查点击是否发生在设置菜单或其触发按钮之外
    if (!toolsMenu.contains(e.target) && !menuToggle.contains(e.target)) {
      toolsMenu.style.display = 'none';
      toolsMenu.setAttribute('aria-hidden', 'true');
      menuToggle.setAttribute('aria-expanded', 'false');
    }
    // 检查点击是否发生在关于浮窗或其触发按钮之外
    if (aboutModalOverlay.classList.contains('active') && !aboutModalOverlay.contains(e.target) && !aboutBtn.contains(e.target)) {
      aboutModalOverlay.classList.remove('active');
      aboutBtn.setAttribute('aria-expanded', 'false');
    }
  });

  // music player interaction
  let controlsVisible = false;
  musicPlayer.addEventListener('click', (e) => {
    controlsVisible = !controlsVisible;
    playerControls.style.display = controlsVisible ? 'flex' : 'none'; // 使用flex以便内部元素对齐
    playerControls.setAttribute('aria-hidden', String(!controlsVisible));
  });

  // play/pause logic
  function setPlayingState(isPlaying) {
    if (isPlaying) {
      // 只有在没有减少动画的情况下才添加旋转类
      if (!currentSettings.reducedMotion) {
        disc.classList.add('playing');
      }
      playPauseBtn.textContent = '⏸';
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

  bgm.addEventListener('play', () => {
    setPlayingState(true);
  });
  bgm.addEventListener('pause', () => {
    setPlayingState(false);
  });
  bgm.addEventListener('ended', () => {
    playNextRandomTrack(); // 播放完毕后随机播放下一首
  });
  bgm.addEventListener('timeupdate', () => {
    if (!bgm.duration || isNaN(bgm.duration)) return;
    const pct = Math.max(0, Math.min(100, (bgm.currentTime / bgm.duration) * 100));
    progressBar.style.width = pct + '%';
    const mm = Math.floor(bgm.currentTime / 60),
      ss = Math.floor(bgm.currentTime % 60);
    timeLabel.textContent = mm + ':' + (ss < 10 ? '0' + ss : ss);
    saveMusicState(); // 实时保存播放进度
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
      // 聚焦到关闭按钮以便键盘操作
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

    // 重新应用设置以确保动画状态正确 (针对 reduced-motion)
    applySettings(loadSettings());
  });

  // Set initial state for disc rotation (if audio already playing)
  if (!bgm.paused && !bgm.ended) setPlayingState(true);
})();