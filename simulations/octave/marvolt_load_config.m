
function cfg = marvolt_load_config()
  % MARVOLT-PA shared configuration loader.
  % Octave 5.1+ requires the json package.
  % Octave 7.1+ includes jsondecode in the core.

  % Check JSON support.
  if exist('jsondecode', 'file') ~= 2 && ...
     exist('jsondecode', 'builtin') ~= 5

    try
      pkg load json;
    catch
      error(['JSON support is unavailable. Install the json package: ' ...
        'pkg install "https://github.com/gnu-octave/pkg-json/archive/v1.6.0.tar.gz"']);
    end_try_catch
  endif

  % Locate the repository.
  this_dir = fileparts(mfilename('fullpath'));
  project_root = fileparts(fileparts(this_dir));
  config_dir = fullfile(project_root, 'config');

  % Configuration files.
  physical_file = fullfile(config_dir, 'physical_parameters.json');
  mechanical_file = fullfile(config_dir, 'mechanical_parameters.json');
  experiment_file = fullfile(config_dir, 'experiment_defaults.json');

  required_files = {physical_file, mechanical_file, experiment_file};

  for idx = 1:numel(required_files)
    if exist(required_files{idx}, 'file') ~= 2
      error('Configuration file not found: %s', required_files{idx});
    endif
  endfor

  % Load configuration structures.
  cfg.physical = jsondecode(fileread(physical_file));
  cfg.mechanical = jsondecode(fileread(mechanical_file));
  cfg.experiment = jsondecode(fileread(experiment_file));

endfunction
