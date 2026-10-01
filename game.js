import * as THREE from 'https://cdn.jsdelivr.net/npm/three@0.160.0/build/three.module.js';

const WIN_SCORE = 12;
const mapSize = 32;

const hud = {
  health: document.getElementById('health'),
  attack: document.getElementById('attack'),
  defense: document.getElementById('defense'),
  wood: document.getElementById('wood'),
  stone: document.getElementById('stone'),
  food: document.getElementById('food'),
  gold: document.getElementById('gold'),
  walls: document.getElementById('walls'),
  score: document.getElementById('score'),
  message: document.getElementById('messageBox'),
  buildBtn: document.getElementById('buildBtn'),
  restartBtn: document.getElementById('restartBtn')
};

const scene = new THREE.Scene();
scene.background = new THREE.Color(0x7bb0d8);
scene.fog = new THREE.Fog(0x7bb0d8, 22, 55);

const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);

const camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.1, 200);
const clock = new THREE.Clock();

const raycaster = new THREE.Raycaster();
const pointer = new THREE.Vector2(0, 0);
const groundPlane = new THREE.Plane(new THREE.Vector3(0, 1, 0), 0);

let buildMode = false;
let gameOver = false;
let won = false;
const pickupColors = {
  W: 0xcaa864,
  S: 0xa0b7cf,
  F: 0x6ee38b,
  G: 0xf7d067
};

const player = {
  radius: 1,
  speed: 7,
  health: 100,
  maxHealth: 100,
  attack: 12,
  defense: 6,
  wood: 0,
  stone: 0,
  food: 4,
  gold: 0,
  walls: 0,
  score: 0,
  kills: 0,
  position: new THREE.Vector3(0, 1, 0),
  yaw: 0,
  pitch: 0.25,
  velocity: new THREE.Vector3(),
  mesh: null,
  lastAttack: 0,
};

const world = {
  pickups: [],
  enemies: [],
  walls: [],
  bullets: [],
  message: 'Collect resources, survive, and win.'
};

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max);
}

function randomRange(min, max) {
  return min + Math.random() * (max - min);
}

function updateHud() {
  hud.health.textContent = `${player.health} / ${player.maxHealth}`;
  hud.attack.textContent = String(player.attack);
  hud.defense.textContent = String(player.defense);
  hud.wood.textContent = String(player.wood);
  hud.stone.textContent = String(player.stone);
  hud.food.textContent = String(player.food);
  hud.gold.textContent = String(player.gold);
  hud.walls.textContent = String(player.walls);
  hud.score.textContent = `${player.score} / ${WIN_SCORE}`;
  hud.message.textContent = world.message;
  hud.buildBtn.textContent = `Build Mode: ${buildMode ? 'On' : 'Off'}`;
}

function setMessage(msg) {
  world.message = msg;
  updateHud();
}

function createGround() {
  const ground = new THREE.Mesh(
    new THREE.PlaneGeometry(mapSize, mapSize),
    new THREE.MeshStandardMaterial({ color: 0x295d3d, roughness: 1 })
  );
  ground.rotation.x = -Math.PI / 2;
  ground.receiveShadow = true;
  scene.add(ground);

  const grid = new THREE.GridHelper(mapSize, mapSize, 0x84e6a3, 0x3d6d56);
  grid.position.y = 0.02;
  scene.add(grid);
}

function addLighting() {
  scene.add(new THREE.HemisphereLight(0xcfe7ff, 0x243728, 1.5));
  const dir = new THREE.DirectionalLight(0xffffff, 1.2);
  dir.position.set(10, 18, 8);
  scene.add(dir);
}

function createPlayerMesh() {
  const body = new THREE.Mesh(
    new THREE.CapsuleGeometry(0.8, 1.5, 4, 8),
    new THREE.MeshStandardMaterial({ color: 0x77f2b1, roughness: 0.8 })
  );
  body.castShadow = true;
  const head = new THREE.Mesh(
    new THREE.SphereGeometry(0.45, 16, 16),
    new THREE.MeshStandardMaterial({ color: 0xf3f7f4 })
  );
  head.position.y = 1.45;
  const weapon = new THREE.Mesh(
    new THREE.BoxGeometry(0.18, 0.18, 1.1),
    new THREE.MeshStandardMaterial({ color: 0x121212 })
  );
  weapon.position.set(0.6, 0.9, 0.7);
  weapon.rotation.x = Math.PI / 2;

  const group = new THREE.Group();
  group.add(body);
  group.add(head);
  group.add(weapon);
  group.position.copy(player.position);
  scene.add(group);
  player.mesh = group;
}

function createPickup(type, x, z) {
  const mesh = new THREE.Mesh(
    new THREE.OctahedronGeometry(0.45, 0),
    new THREE.MeshStandardMaterial({ color: pickupColors[type], emissive: pickupColors[type], emissiveIntensity: 0.25 })
  );
  mesh.position.set(x, 0.8, z);
  scene.add(mesh);
  world.pickups.push({ type, mesh, x, z });
}

function spawnPickups() {
  for (let i = 0; i < 18; i++) {
    const type = ['W', 'S', 'F', 'G'][Math.floor(Math.random() * 4)];
    createPickup(type, randomRange(-mapSize / 2 + 2, mapSize / 2 - 2), randomRange(-mapSize / 2 + 2, mapSize / 2 - 2));
  }
}

function createEnemy(x, z) {
  const enemy = new THREE.Mesh(
    new THREE.BoxGeometry(1.4, 1.8, 1.4),
    new THREE.MeshStandardMaterial({ color: 0xff4d4d, emissive: 0x771111 })
  );
  enemy.position.set(x, 1.1, z);
  scene.add(enemy);

  world.enemies.push({
    mesh: enemy,
    x,
    z,
    health: 30 + Math.random() * 20,
    attack: 8 + Math.random() * 7,
    speed: 2.2 + Math.random() * 1.3,
    alive: true,
  });
}

function spawnEnemies() {
  for (let i = 0; i < 6; i++) {
    createEnemy(randomRange(-12, 12), randomRange(-12, 12));
  }
}

function placeWall(x, z) {
  if (player.wood < 3 || player.stone < 2) {
    setMessage('Need 3 wood and 2 stone to build a wall.');
    return;
  }

  const wallMesh = new THREE.Mesh(
    new THREE.BoxGeometry(2.2, 2.2, 2.2),
    new THREE.MeshStandardMaterial({ color: 0x8db3ff, metalness: 0.3, roughness: 0.8 })
  );
  wallMesh.position.set(x, 1.1, z);
  scene.add(wallMesh);

  world.walls.push({ mesh: wallMesh, x, z, health: 60 });
  player.wood -= 3;
  player.stone -= 2;
  player.walls += 1;
  player.defense += 1;
  setMessage('Wall constructed. Defense increased.');
  updateHud();
}

function tryBuildFromView() {
  if (!buildMode) return;

  const origin = camera.position.clone();
  const direction = camera.getWorldDirection(new THREE.Vector3());
  const ray = new THREE.Raycaster(origin, direction);
  const hit = ray.ray.intersectPlane(groundPlane, new THREE.Vector3());
  if (hit) {
    placeWall(hit.x, hit.z);
  }
}

function handleInput() {
  const moveDir = new THREE.Vector3();
  const forward = new THREE.Vector3(Math.sin(player.yaw), 0, Math.cos(player.yaw));
  const right = new THREE.Vector3(forward.z, 0, -forward.x);

  if (keys['KeyW']) moveDir.add(forward);
  if (keys['KeyS']) moveDir.sub(forward);
  if (keys['KeyA']) moveDir.sub(right);
  if (keys['KeyD']) moveDir.add(right);

  if (moveDir.lengthSq() > 0) {
    moveDir.normalize().multiplyScalar(player.speed * deltaTime);
    player.position.add(moveDir);
    player.position.x = clamp(player.position.x, -mapSize / 2 + 1, mapSize / 2 - 1);
    player.position.z = clamp(player.position.z, -mapSize / 2 + 1, mapSize / 2 - 1);
  }

  player.mesh.position.copy(player.position);
}

function updateCamera() {
  const cameraOffset = new THREE.Vector3(
    Math.sin(player.yaw) * 7,
    4.5 + Math.sin(player.pitch) * 2,
    Math.cos(player.yaw) * 7
  );

  camera.position.copy(player.position).add(cameraOffset);
  camera.lookAt(player.position.x, player.position.y + 1.5, player.position.z);
}

function collectPickups() {
  for (let i = world.pickups.length - 1; i >= 0; i--) {
    const pickup = world.pickups[i];
    const dist = pickup.mesh.position.distanceTo(player.position);
    if (dist < 1.5) {
      if (pickup.type === 'W') player.wood += 2;
      if (pickup.type === 'S') player.stone += 2;
      if (pickup.type === 'F') player.food += 3;
      if (pickup.type === 'G') player.gold += 2;

      player.score += 1;
      scene.remove(pickup.mesh);
      world.pickups.splice(i, 1);
      setMessage('Resource collected!');
      updateHud();

      if (player.score >= WIN_SCORE) {
        won = true;
        setMessage('You survived and won the world! Press R or Restart.');
      }
    }
  }
}

function shoot() {
  if (gameOver || won) return;

  const direction = new THREE.Vector3();
  camera.getWorldDirection(direction);
  const origin = camera.position.clone().add(direction.clone().multiplyScalar(1.2));

  const bullet = new THREE.Mesh(
    new THREE.SphereGeometry(0.14, 8, 8),
    new THREE.MeshStandardMaterial({ color: 0xfff3a5, emissive: 0xffdd5a })
  );
  bullet.position.copy(origin);
  scene.add(bullet);

  world.bullets.push({
    mesh: bullet,
    velocity: direction.clone().multiplyScalar(28),
    life: 1.4,
  });
}

function updateBullets() {
  for (let i = world.bullets.length - 1; i >= 0; i--) {
    const bullet = world.bullets[i];
    bullet.mesh.position.addScaledVector(bullet.velocity, deltaTime);
    bullet.life -= deltaTime;

    if (bullet.life <= 0) {
      scene.remove(bullet.mesh);
      world.bullets.splice(i, 1);
      continue;
    }

    for (let j = world.enemies.length - 1; j >= 0; j--) {
      const enemy = world.enemies[j];
      if (!enemy.alive) continue;
      const dist = bullet.mesh.position.distanceTo(enemy.mesh.position);
      if (dist < 1.2) {
        enemy.health -= 22;
        scene.remove(bullet.mesh);
        world.bullets.splice(i, 1);

        if (enemy.health <= 0) {
          scene.remove(enemy.mesh);
          enemy.alive = false;
          player.gold += 2;
          player.kills += 1;
          world.enemies.splice(j, 1);
          if (world.enemies.length < 6 && Math.random() < 0.7) {
            createEnemy(randomRange(-12, 12), randomRange(-12, 12));
          }
          setMessage('Enemy defeated!');
        }
        break;
      }
    }
  }
}

function updateEnemies() {
  for (const enemy of world.enemies) {
    if (!enemy.alive) continue;

    const dx = player.position.x - enemy.mesh.position.x;
    const dz = player.position.z - enemy.mesh.position.z;
    const dist = Math.hypot(dx, dz);

    if (dist > 0.1) {
      const stepX = (dx / dist) * enemy.speed * deltaTime;
      const stepZ = (dz / dist) * enemy.speed * deltaTime;
      enemy.mesh.position.x += stepX;
      enemy.mesh.position.z += stepZ;
    }

    if (dist < 1.6) {
      const damage = Math.max(1, enemy.attack - player.defense + Math.random() * 4);
      player.health -= damage * deltaTime * 2.8;
    }
  }

  if (player.health <= 0) {
    player.health = 0;
    gameOver = true;
    setMessage('You were defeated. Press R or Restart.');
  }

  updateHud();
}

function updatePickups() {
  if (world.pickups.length < 18) {
    createPickup(['W', 'S', 'F', 'G'][Math.floor(Math.random() * 4)], randomRange(-14, 14), randomRange(-14, 14));
  }
}

function setupInput() {
  const keys = {};
  window.addEventListener('keydown', (event) => {
    keys[event.code] = true;

    if (event.code === 'KeyB') {
      buildMode = !buildMode;
      setMessage(buildMode ? 'Build mode on. Click to place wall.' : 'Build mode off.');
    }

    if (event.code === 'KeyH') {
      if (player.food > 0) {
        player.food -= 1;
        player.health = Math.min(player.maxHealth, player.health + 14);
        setMessage('Healed for 14 health.');
        updateHud();
      } else {
        setMessage('No food left.');
      }
    }

    if (event.code === 'KeyR') {
      location.reload();
    }
  });

  window.addEventListener('keyup', (event) => {
    keys[event.code] = false;
  });

  document.addEventListener('pointerlockchange', () => {
    if (document.pointerLockElement === renderer.domElement) {
      setMessage('Pointer lock active.');
    }
  });

  renderer.domElement.addEventListener('click', () => {
    if (!document.pointerLockElement) {
      renderer.domElement.requestPointerLock();
    } else if (!buildMode) {
      shoot();
    } else {
      tryBuildFromView();
    }
  });

  document.addEventListener('mousemove', (event) => {
    if (document.pointerLockElement === renderer.domElement) {
      player.yaw -= event.movementX * 0.0025;
      player.pitch -= event.movementY * 0.002;
      player.pitch = clamp(player.pitch, -1.2, 1.1);
    }
  });

  return keys;
}

const keys = setupInput();
let deltaTime = 0;

function createWorld() {
  createGround();
  addLighting();
  createPlayerMesh();
  spawnPickups();
  spawnEnemies();
  updateHud();
}

function animate() {
  requestAnimationFrame(animate);
  deltaTime = Math.min(clock.getDelta(), 0.033);

  if (!gameOver && !won) {
    handleInput();
    collectPickups();
    updateEnemies();
    updateBullets();
    updatePickups();
  }

  updateCamera();
  renderer.render(scene, camera);
}

createWorld();
animate();
window.addEventListener('resize', () => {
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(window.innerWidth, window.innerHeight);
});

hud.restartBtn.addEventListener('click', () => location.reload());
hud.buildBtn.addEventListener('click', () => {
  buildMode = !buildMode;
  setMessage(buildMode ? 'Build mode on. Click to place wall.' : 'Build mode off.');
});
